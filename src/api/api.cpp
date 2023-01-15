#include "api.hpp"
#include "api/util/env.hpp"
#include <GL/glew.h>
#include <amogpu/amogpu.hpp>
#include "api/util/file.hpp"
#include "api/event/event.hpp"

profile api::app {};
float api::dt {};

void api::path(const char *p_arg_path) {
    util::log(p_arg_path);
}

void api::mainloop(feature *p_scene_initial) {
    util::log("Initialising");

    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        util::log("Failed to init SDL");
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    api::app.p_sdl_window = SDL_CreateWindow("Anubis Graphics Kit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, api::app.screen_width, api::app.screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(api::app.p_sdl_window)};
    util::log("SDL window released");

    glewExperimental = true;
    glewInit();

    SDL_GL_SetSwapInterval(1);
    util::log("OpenGL 4 context created");

    amogpu::gl_version = "#version 450 core";
    amogpu::init();

    util::timing reduce_cpu_ticks_timing {};
    util::timing counter_fps_timing {};

    uint64_t cpu_ticks_interval {1000 / api::app.fps}, cpu_ticks_now {}, cpu_ticks_last {};
    uint64_t fps_interval {1000};
    uint64_t ticked_frames {};
    SDL_Event sdl_event {};
    glm::vec3 previous_camera_rotation {2, 2, 2};

    api::app.p_world_client = new ::world {};
    api::gc::create(api::app.p_world_client);
    util::log("Initialising world client...");

    api::app.p_world_renderer = new renderer {};
    api::gc::create(api::app.p_world_renderer);
    util::log("Initialising world renderer...");

    api::app.p_current_camera = new camera {};
    api::world::create(api::app.p_current_camera);
    util::log("Initialising world camera frustum...");

    api::app.p_current_player = new entity {};
    api::world::create(api::app.p_current_player);
    util::log("Initialising world default player...");

    api::scene::load(p_scene_initial);
    util::log("Initialising main scene...");

    /* Flush all object. */
    api::app.p_current_camera->process_perspective(api::app.screen_width, api::app.screen_height);
    api::app.garbage_collector.do_update();

    api::app.mainloop = true;
    util::log("Anubis Graphics Kit initialised with successfully!");

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
                    api::app.p_world_client->on_event(sdl_event);
                    api::app.p_world_renderer->on_event(sdl_event);

                    for (feature *&p_feature : api::app.loaded_service_list) {
                        p_feature->on_event(sdl_event);
                    }
                    break;
                }
            }
        }

        if (util::reset_when(reduce_cpu_ticks_timing, fps_interval)) {
            api::app.display_fps = ticked_frames;
            ticked_frames = 0;
        }

        if (api::app.p_current_scene != nullptr) {
            api::app.p_current_scene->on_update();
        }

        for (feature *&p_feature : api::app.loaded_service_list) {
            p_feature->on_update();
        }

        api::app.p_world_client->on_update();
        api::app.input_manager.on_update();
        api::app.garbage_collector.do_update();

        amogpu::matrix();

        glViewport(0, 0, api::app.screen_width, api::app.screen_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        api::app.p_world_renderer->on_render();

        if (api::app.p_current_scene != nullptr) {
            api::app.p_current_scene->on_render();
        }

        for (feature *&p_feature : api::app.loaded_service_list) {
            p_feature->on_render();
        }

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

feature *&api::scene::current() {
    return api::app.p_current_scene;
}

void api::gc::destroy(feature *p_target) {
    api::app.garbage_collector.destroy(p_target);
}

void api::gc::create(feature *p_target) {
    api::app.garbage_collector.create(p_target);
}

bool api::shading::create_program(std::string_view tag, ::shading::program *p_program, const std::vector<::shading::resource> &resource_list) {
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
            flag = util::read_file(resource.path.data(), shader_source);
        }

        if (shader_source.empty()) {
            util::log("Invalid shader resource code!");
            util::log(resource.path.data());
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
            util::log(tag.data());
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
            util::log(std::string(tag.data()) + " shading program successfully linked");
            api::shading::registry(tag, p_program);
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
    api::app.shader_registry_map[key.data()] = p_program;
    return p_program;
}

camera *&api::world::current_camera() {
    return api::app.p_current_camera;
}

::world *&api::world::get() {
    return api::app.p_world_client;
}

renderer *&api::world::renderer() {
    return api::app.p_world_renderer;
}

void api::world::create(world_feature *p_world_feature) {
    api::app.p_world_client->registry_wf(p_world_feature);
}

void api::world::destroy(world_feature *p_world_feature) {
    api::app.p_world_client->unregister_wf(p_world_feature);
}

model *api::world::create(std::string_view tag, std::string_view path, ::mesh::format format) {
    ::mesh::data mesh {format};
    if (api::mesh::load(mesh, path.data())) {
        return nullptr;
    }

    return api::app.p_world_renderer->add(tag, mesh);
}

entity *&api::world::current_player() {
    return api::app.p_current_player;
}

bool api::mesh::load(::mesh::data &data, std::string_view path) {
    return api::app.mesher_loader.load_object(data, path);
}

mesh_loader &api::mesh::loader() {
    return api::app.mesher_loader;
}

bool api::input::pressed(std::string_view input_tag) {
    return api::app.input_manager.input_map[input_tag.data()];
}

void api::service::registry(feature *p_feature) {
    if (p_feature == nullptr) {
        return;
    }

    api::gc::create(p_feature);
    api::app.loaded_service_list.push_back(p_feature);

    p_feature->id = (int32_t) api::app.loaded_service_list.size();
    util::log("Registered internal service ID (" + std::to_string(p_feature->id) + ")");
}
