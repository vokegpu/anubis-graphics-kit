#include "api.hpp"
#include "api/util/env.hpp"
#include <GL/glew.h>
#include "api/util/file.hpp"
#include "api/event/event.hpp"

profile api::app {};
float api::dt {};

void api::path(const char *p_arg_path) {
    util::log(p_arg_path);
}

void api::mainloop(feature *p_scene_initial) {
    util::log("Initialising.");

    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        util::log("Failed to init SDL!");
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    api::app.p_sdl_window = SDL_CreateWindow("Anubis Graphics Kit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, api::app.screen_width, api::app.screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(api::app.p_sdl_window)};
    util::log("SDL window released.");

    glewExperimental = true;
    glewInit();

    SDL_GL_SetSwapInterval(1);
    util::log("OpenGL 4 context created.");

    SDL_Surface *p_surf {SDL_LoadBMP("./data/icon.bmp")};
    SDL_SetWindowIcon(api::app.p_sdl_window, p_surf);
    SDL_FreeSurface(p_surf);

    util::timing reduce_cpu_ticks_timing {};
    api::app.setting.init();

    uint64_t cpu_ticks_interval {1000 / api::app.fps}, cpu_ticks_now {SDL_GetPerformanceCounter()}, cpu_ticks_last {};
    uint64_t fps_interval {1000};
    uint64_t ticked_frames {};
    uint64_t cpu_performance_frequency {1};

    SDL_Event sdl_event {};
    glm::vec3 previous_camera_rotation {2, 2, 2};

    api::app.p_asset_manager_service = new asset_manager {};
    api::task::synchronize(api::app.p_asset_manager_service);
    util::log("Asset manager created.");

    api::app.p_input_service = new ::input {};
    api::task::synchronize(api::app.p_input_service);
    util::log("Input manager created.");

    api::app.p_world_service = new ::world {};
    api::task::synchronize(api::app.p_world_service);
    util::log("World client created.");

    api::app.p_renderer_service = new renderer {};
    api::task::synchronize(api::app.p_renderer_service);
    util::log("World renderer created.");

    api::app.p_curr_camera = new camera {};
    api::world::create(api::app.p_curr_camera);
    util::log("Main camera frustum created.");

    api::app.p_curr_player = new entity {};
    api::world::create(api::app.p_curr_player);
    util::log("Main world player created.");

    api::app.p_world_time_service = new world_time_manager {};
    api::task::synchronize(api::app.p_world_time_service);
    util::log("World time manager created.");

    /* Flush all object. */
    api::task::populate();
    api::app.p_curr_camera->process_perspective(api::app.screen_width, api::app.screen_height);
    api::app.p_renderer_service->process_framebuffer(api::app.screen_width, api::app.screen_height);
    util::log("All feature objects were synchronized.");

    api::scene::load(p_scene_initial);
    util::log("Loading initial scene!");

    api::app.mainloop = true;
    util::log("Anubis Graphics Kit initialised successfully!");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    while (api::app.mainloop) {
        cpu_ticks_last = cpu_ticks_now;
        cpu_ticks_now = SDL_GetPerformanceCounter();
        cpu_performance_frequency += SDL_GetPerformanceFrequency();

        api::dt = static_cast<float>(cpu_ticks_now - cpu_ticks_last) / static_cast<float>(cpu_performance_frequency);
        cpu_performance_frequency = 0;

        while (SDL_PollEvent(&sdl_event)) {
            switch (sdl_event.type) {
                case SDL_QUIT: {
                    api::app.mainloop = false;
                    util::log("Preparing to shutdown AGK...");
                    break;
                }

                default: {
                    api::app.p_input_service->on_event(sdl_event);
                    if (api::app.p_curr_scene != nullptr) api::app.p_curr_scene->on_event(sdl_event);
                    api::app.p_world_service->on_event(sdl_event);
                    api::app.p_renderer_service->on_event(sdl_event);

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

        if (api::app.p_curr_scene != nullptr) {
            api::app.p_curr_scene->on_update();
        }

        for (feature *&p_feature : api::app.loaded_service_list) {
            p_feature->on_update();
        }

        api::app.p_world_time_service->on_update();
        api::app.p_world_service->on_update();
        api::app.p_input_service->on_update();
        api::task::populate();

        glViewport(0, 0, api::app.screen_width, api::app.screen_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(api::app.background[0], api::app.background[1], api::app.background[2], 1.0f);

        api::app.p_renderer_service->on_render();

        if (api::app.p_curr_scene != nullptr) {
            api::app.p_curr_scene->on_render();
        }

        for (feature *&p_feature : api::app.loaded_service_list) {
            p_feature->on_render();
        }

        ticked_frames++;
        SDL_GL_SwapWindow(api::app.p_sdl_window);
        SDL_Delay(cpu_ticks_interval);
    }

    util::log("Anubis Graphics Kit shutdown! ^^");
}

void api::viewport() {
    glViewport(0, 0, api::app.screen_width, api::app.screen_height);
}

void api::scene::load(feature *scene) {
    auto &current_scene {api::scene::current()};

    if (scene == nullptr && current_scene != nullptr) {
        current_scene->on_destroy();
        delete current_scene;
        api::app.p_curr_scene = nullptr;
        return;
    }

    if (scene != nullptr && current_scene != nullptr && current_scene == scene) {
        current_scene->on_destroy();
        delete current_scene;
        return;
    }

    if (scene != nullptr && current_scene != nullptr && current_scene != scene) {
        current_scene->on_destroy();
        delete current_scene;
        scene->on_create();
        api::app.p_curr_scene = scene;
        return;
    }

    if (scene != nullptr && current_scene == nullptr) {
        scene->on_create();
        api::app.p_curr_scene = scene;
    }
}

feature *&api::scene::current() {
    return api::app.p_curr_scene;
}

camera *&api::world::current_camera() {
    return api::app.p_curr_camera;
}

::world *&api::world::get() {
    return api::app.p_world_service;
}

renderer *&api::world::renderer() {
    return api::app.p_renderer_service;
}

void api::world::create(world_feature *p_world_feature) {
    api::app.p_world_service->registry_wf(p_world_feature);
}

void api::world::destroy(world_feature *p_world_feature) {
    api::app.p_world_service->unregister_wf(p_world_feature);
}

model *api::world::create(std::string_view tag, std::string_view path, ::mesh::format format) {
    ::mesh::data mesh {format};
    return api::world::create(tag, path, mesh);
}

model *api::world::create(std::string_view tag, std::string_view path, ::mesh::data &mesh) {
    if (api::mesh::load(mesh, path.data())) {
        return nullptr;
    }

    return api::app.p_renderer_service->add(tag, mesh);
}

entity *&api::world::current_player() {
    return api::app.p_curr_player;
}

world_time_manager *&api::world::time_manager() {
     return api::app.p_world_time_service;
}

bool api::mesh::load(::mesh::data &data, std::string_view path) {
    return api::app.mesh_loader_manager.load_object(data, path);
}

mesh_loader &api::mesh::loader() {
    return api::app.mesh_loader_manager;
}

bool api::input::pressed(std::string_view input_tag) {
    return api::app.p_input_service->input_map[input_tag.data()];
}

void api::task::registry(feature *p_feature) {
    if (p_feature == nullptr) {
        return;
    }

    api::task::synchronize(p_feature);
    api::app.loaded_service_list.push_back(p_feature);

    p_feature->id = (int32_t) api::app.loaded_service_list.size();
    util::log("Registered internal service ID (" + std::to_string(p_feature->id) + ")");
}

void api::task::populate() {
    while (!api::app.task_queue.empty()) {
        auto &p_feature {api::app.task_queue.front()};
        if (p_feature != nullptr && p_feature->is_dead) {
            p_feature->on_destroy();
        } else if (p_feature != nullptr) {
            p_feature->on_create();
        }

        api::app.task_queue.pop();
    }
}

void api::task::synchronize(feature *p_feature) {
    api::app.task_queue.push(p_feature);
}

feature *api::asset::find(std::string_view asset_name) {
    return api::app.p_asset_manager_service->find(asset_name);
}
