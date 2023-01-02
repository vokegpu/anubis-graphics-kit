#include "api.hpp"
#include "api/util/env.hpp"
#include <GL/glew.h>
#include <amogpu/amogpu.hpp>
#include "api/util/file.hpp"

core api::app {};
float api::dt {};

void api::path(const char* str_path) {
    util::log(str_path);
}

void api::mainloop(feature* initial_scene) {
    util::log("initialising");

    if (SDL_Init(SDL_INIT_VIDEO)) {
        util::log("failed to init SDL");
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    api::app.p_sdl_window = SDL_CreateWindow("Anubis Graphics Kit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, api::app.screen_width, api::app.screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(api::app.p_sdl_window)};
    util::log("window released");

    glewExperimental = true;
    glewInit();
    SDL_GL_SetSwapInterval(1);
    util::log("OpenGL 4 context created");

    amogpu::gl_version = "#version 450 core";
    amogpu::init();

    dynamic_batching batch {};
    font_renderer f_renderer {};
    f_renderer.load("./data/fonts/impact.ttf", 36);
    f_renderer.from(amogpu::invoked);
    batch.set_frustum_depth(false);

    util::timing reduce_cpu_ticks_timing {};
    util::timing counter_fps_timing {};

    uint64_t cpu_ticks_interval {1000 / api::app.fps}, cpu_ticks_now {}, cpu_ticks_last {};
    uint64_t fps_interval {1000};
    uint64_t ticked_frames {};
    SDL_Event sdl_event {};
    glm::vec3 previous_camera_rotation {};

    api::app.mainloop = true;
    api::gc::create(&api::app.world_client);
    api::scene::load(initial_scene);

    api::app.p_current_camera = new camera {};
    api::gc::create(api::app.p_current_camera);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    while (api::app.mainloop) {
        cpu_ticks_last = cpu_ticks_now;
        cpu_ticks_now = SDL_GetPerformanceCounter();
        api::dt = static_cast<float>(cpu_ticks_now - cpu_ticks_last) / static_cast<float>(SDL_GetPerformanceFrequency());
        api::dt *= 100.0f;

        while (SDL_PollEvent(&sdl_event)) {
            switch (sdl_event.type) {
                case SDL_QUIT: {
                    api::app.mainloop = false;
                    break;
                }

                default: {
                    api::app.input_manager.on_event(sdl_event);
                    if (api::app.p_current_scene != nullptr) api::app.p_current_scene->on_event(sdl_event);
                    api::app.p_current_camera->on_event(sdl_event);
                    api::app.world_client.on_event(sdl_event);
                    break;
                }
            }
        }

        if (util::resetifreach(reduce_cpu_ticks_timing, fps_interval)) {
            api::app.display_fps = ticked_frames;
            ticked_frames = 0;
        }

        if (api::app.p_current_scene != nullptr) {
            api::app.p_current_scene->on_update();
        }

        api::app.world_client.on_update();
        api::app.garbage_collector.do_update();
        api::app.input_manager.on_update();
        amogpu::matrix();

        glViewport(0, 0, api::app.screen_width, api::app.screen_height);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        if (api::app.p_current_scene != nullptr) {
            api::app.p_current_scene->on_render();
        }

        if (api::app.p_current_camera->rotation != previous_camera_rotation) {
            previous_camera_rotation = api::app.p_current_camera->rotation;

            batch.invoke();
            f_renderer.render("Yaw Pitch [" + std::to_string(previous_camera_rotation.x) + ", " + std::to_string(previous_camera_rotation.y) + "]", 10, 10, {1.0f, 1.0f, 1.0f, 1.0f});
            batch.revoke();
        }

        batch.draw();

        ticked_frames++;
        SDL_GL_SwapWindow(api::app.p_sdl_window);
        SDL_Delay(cpu_ticks_interval);
    }
}

void api::scene::load(feature* scene) {
    auto &current_scene {api::scene::current()};

    // if you think it is bad, you should code more.

    if (scene == nullptr && current_scene != nullptr) {
        api::gc::destroy(current_scene);
        api::app.p_current_scene = nullptr;
        return;
    }

    if (scene != nullptr && current_scene != nullptr && current_scene == scene) {
        api::gc::destroy(scene);
        return;
    }

    if (scene != nullptr && current_scene != nullptr && current_scene != scene) {
        api::gc::destroy(current_scene);
        api::gc::create(scene);
        api::app.p_current_scene = scene;
        return;
    }

    if (scene != nullptr && current_scene == nullptr) {
        api::gc::create(scene);
        api::app.p_current_scene = scene;
    }
}

feature* &api::scene::current() {
    return api::app.p_current_scene;
}

void api::gc::destroy(feature *p_target) {
    api::app.garbage_collector.destroy(p_target);
}

void api::gc::create(feature *p_target) {
    api::app.garbage_collector.create(p_target);
}

bool api::shading::createprogram(std::string_view name, ::shading::program *p_program, const std::vector<::shading::resource> &resource_list) {
    bool flag {!resource_list.empty()};
    uint32_t shader {};
    const char *p_shader_resource {};
    std::string shader_source {};
    std::vector<uint32_t> compiled_shader_list {};

    for (const ::shading::resource &resource : resource_list) {
        shader_source.clear();

        if (resource.is_source_code) {
            shader_source = resource.path.data();
        } else {
            flag = util::readfile(resource.path.data(), shader_source);
        }

        if (shader_source.empty()) {
            break;
        }

        p_shader_resource = shader_source.data();

        shader = glCreateShader(resource.stage);
        glShaderSource(shader, 1, &p_shader_resource, nullptr);
        glCompileShader(shader);

        GLint status {};
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if (!status) {
            char log[256];
            glGetShaderInfoLog(shader, 256, nullptr, log);
            util::log("Failed to compile shader!");
            util::log(log);
            return false;
        }

        if (!flag) {
            util::log(name.data());
            break;
        }

        glAttachShader(p_program->id, shader);
        compiled_shader_list.push_back(shader);
    }

    if (flag) {
        GLint status {};
        glLinkProgram(p_program->id);
        glGetProgramiv(p_program->id, GL_LINK_STATUS, &status);

        if (!status) {
            char log[256];
            glGetProgramInfoLog(p_program->id, 256, nullptr, log);
            util::log("Failed to link this shading program!");
            util::log(log);
        } else {
            util::log(std::string(name.data()) + " shading program successfully linked");
            api::shading::registry(name, p_program);
        }

        return status;
    }

    for (uint32_t &compiled_shaders : compiled_shader_list) {
        glDeleteShader(compiled_shaders);
    }

    return false;
}

bool api::shading::find(std::string_view key, ::shading::program *&p_program) {
    p_program = api::app.shader_registry_map[key.data()];
    return p_program != nullptr;
}

::shading::program *api::shading::registry(std::string_view key, ::shading::program *p_program) {
    return api::app.shader_registry_map[key.data()];
}

camera *api::world::currentcamera() {
    return api::app.p_current_camera;
}

::world &api::world::current() {
    return api::app.world_client;
}

bool api::mesh::load(::mesh::data &data, std::string_view path) {
    return api::app.mesh3d_loader.load_object(data, path);
}

bool api::input::pressed(std::string_view input_tag) {
    return api::app.input_manager.input_map[input_tag.data()];
}
