#include "agk.hpp"
#include "util/env.hpp"
#include <GL/glew.h>
#include "util/file.hpp"
#include "event/event.hpp"

core agk::app {};
float agk::dt {};

void agk::path(const char *p_arg_path) {
    util::log(p_arg_path);
}

void agk::mainloop(imodule *p_scene_initial) {
    util::log("Initialising");

    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        util::log("Failed to init SDL");
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    agk::app.p_sdl_window = SDL_CreateWindow("Anubis Graphics Kit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, agk::app.screen_width, agk::app.screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GL_CreateContext(agk::app.p_sdl_window);
    util::log("SDL window released");

    glewExperimental = true;
    glewInit();

    SDL_GL_SetSwapInterval(0);
    util::log("OpenGL 4 context created");

    SDL_Surface *p_surf {SDL_LoadBMP("./data/icon.bmp")};
    SDL_SetWindowIcon(agk::app.p_sdl_window, p_surf);
    SDL_FreeSurface(p_surf);

    util::timing reduce_cpu_ticks_timing {};
    agk::app.setting.init();

    uint64_t cpu_ticks_interval {1000 / agk::app.fps}, cpu_ticks_now {SDL_GetPerformanceCounter()}, cpu_ticks_last {};
    uint64_t fps_interval {1000};
    uint64_t ticked_frames {};
    uint64_t cpu_performance_frequency {1};
    cpu_ticks_interval = 0;

    SDL_Event sdl_event {};
    glm::vec3 previous_camera_rotation {2, 2, 2};

    agk::app.p_asset_manager_service = new asset_manager {};
    agk::task::synchronize(agk::app.p_asset_manager_service);
    util::log("Asset manager created.");

    agk::app.p_input_service = new ::input {};
    agk::task::registry(agk::app.p_input_service, agk::service::updateable | agk::service::listenable);
    util::log("Input manager created");

    agk::app.p_world_service = new ::world {};
    agk::task::registry(agk::app.p_world_service, agk::service::updateable | agk::service::listenable | agk::service:::renderable);
    util::log("World client created");

    agk::app.p_renderer_service = new renderer {};
    agk::task::registry(agk::app.p_renderer_service, agk::service::updateable | agk::service::listenable | agk::service:::renderable);
    util::log("World renderer created");

    agk::app.p_curr_camera = new camera {};
    agk::world::registry(agk::app.p_curr_camera);
    util::log("Main camera frustum created");

    agk::app.p_curr_player = new entity {};
    agk::world::create(agk::app.p_curr_player);
    util::log("Main world player created");

    agk::app.p_sky = new sky {};
    agk::task::registry(agk::app.p_sky, agk::service::updateable | agk::service::listenable | agk::service:::renderable);
    util::log("World time manager created");

    agk::app.p_user_camera {new usercamera {}};
    agk::task::registry(agk::app.p_user_camera, agk::service::updateable | agk::service::listenable);
    util::log("User camera manager created");

    /* Flush all object. */
    agk::task::populate();
    agk::app.p_curr_camera->process_perspective(agk::app.screen_width, agk::app.screen_height);
    agk::app.p_renderer_service->process_framebuffer(agk::app.screen_width, agk::app.screen_height);
    util::log("All feature objects were synchronized");

    agk::scene::load(p_scene_initial);
    util::log("Loading initial scene");

    agk::app.mainloop = true;
    util::log("Anubis Graphics Kit initialised successfully");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    while (agk::app.mainloop) {
        cpu_ticks_last = cpu_ticks_now;
        cpu_ticks_now = SDL_GetPerformanceCounter();
        cpu_performance_frequency += SDL_GetPerformanceFrequency();

        agk::dt = static_cast<float>(cpu_ticks_now - cpu_ticks_last) / static_cast<float>(cpu_performance_frequency);
        cpu_performance_frequency = 0;

        while (SDL_PollEvent(&sdl_event)) {
            switch (sdl_event.type) {
                case SDL_QUIT: {
                    agk::app.mainloop = false;
                    util::log("Preparing to shutdown AGK");
                    break;
                }

                default: {
                    for (imodule *&p_services : core::listenablelist) {
                        p_services->on_event(sdl_event);
                    }

                    if (agk::app.p_curr_scene != nullptr) {
                        agk::app.p_curr_scene->on_event(sdl_event);
                    }
                    break;
                }
            }
        }

        if (util::reset_when(reduce_cpu_ticks_timing, fps_interval)) {
            agk::app.display_fps = ticked_frames;
            ticked_frames = 0;
        }

        for (imodule *&p_services : core::updateablelist) {
            p_services->on_update();
        }

        if (agk::app.p_curr_scene != nullptr) {
            agk::app.p_curr_scene->on_update();
        }

        agk::task::populate();

        glViewport(0, 0, agk::app.screen_width, agk::app.screen_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(agk::app.background[0], agk::app.background[1], agk::app.background[2], 1.0f);

        for (imodule *&p_services : core::renderablelist) {
            p_services->on_render();
        }

        if (agk::app.p_curr_scene != nullptr) {
            agk::app.p_curr_scene->on_render();
        }

        ticked_frames++;
        SDL_GL_SwapWindow(agk::app.p_sdl_window);

        if (agk::app.vsync) {
            SDL_Delay(agk::app.fps / 1000);
        }
    }

    util::log("Anubis Graphics Kit shutdown! ^^");
}

void agk::viewport() {
    glViewport(0, 0, agk::app.screen_width, agk::app.screen_height);
}

void agk::setfps(uint32_t fps, bool vsync) {
    if (agk::app.vsync != vsync) {
        agk::app.vsync = vsync;
        SDL_GL_SetSwapInterval(vsync);
    }

    agk::app.fps = fps;
}

void agk::scene::load(imodule *p_scene) {
    auto &current_scene {agk::scene::current()};

    if (p_scene == nullptr && current_scene != nullptr) {
        current_scene->on_destroy();
        delete current_scene;
        agk::app.p_curr_scene = nullptr;
        return;
    }

    if (p_scene != nullptr && current_scene != nullptr && current_scene == p_scene) {
        current_scene->on_destroy();
        delete current_scene;
        return;
    }

    if (p_scene != nullptr && current_scene != nullptr && current_scene != p_scene) {
        current_scene->on_destroy();
        delete current_scene;
        p_scene->on_create();
        agk::app.p_curr_scene = p_scene;
        return;
    }

    if (p_scene != nullptr && current_scene == nullptr) {
        p_scene->on_create();
        agk::app.p_curr_scene = p_scene;
    }
}

imodule *&agk::scene::current() {
    return agk::app.p_curr_scene;
}

camera *&agk::world::current_camera() {
    return agk::app.p_curr_camera;
}

::world *&agk::world::get() {
    return agk::app.p_world_service;
}

renderer *&agk::world::renderer() {
    return agk::app.p_renderer_service;
}

void agk::world::create(object *p_object) {
    agk::app.p_world_service->registry(p_object);
}

void agk::world::destroy(object *p_object) {
    agk::app.p_world_service->erase(p_object);
}

entity *&agk::world::current_player() {
    return agk::app.p_curr_player;
}

sky *&agk::world::sky() {
    return agk::app.p_sky;
}

bool agk::mesh::load(::mesh::data &data, std::string_view path) {
    return agk::app.mesh_loader_manager.load_object(data, path);
}

meshloader &agk::mesh::loader() {
    return agk::app.mesh_loader_manager;
}

bool agk::ui::input(std::string_view input_tag) {
    return agk::app.p_input_service->input_map[input_tag.data()];
}

usercamera *&agk::ui::get_user_camera() {
    return agk::app.p_user_camera;
}

void agk::task::registry(imodule *p_service, uint16_t flags) {
    if (p_service == nullptr) {
        return;
    }

    agk::task::synchronize(p_service);
    p_service->id = ++core::token;

    if (agk::flags::contains(flags, agk::service::renderable)) {
        core::renderablelist.push_back(p_service);
    }

    if (agk::flags::contains(flags, agk::service::updateable)) {
        core::updateablelist.push_back(p_service);
    }

    if (agk::flags::contains(flags, agk::service::listenable)) {
        core::listenablelist.push_back(p_service);
    }
}

void agk::task::populate() {
    while (!core::taskqueue.empty()) {
        auto &p_feature {core::taskqueue.front()};
        if (p_feature != nullptr && p_feature->is_dead) {
            p_feature->on_destroy();
        } else if (p_feature != nullptr) {
            p_feature->on_create();
        }

        core::taskqueue.pop();
    }
}

void agk::task::synchronize(imodule *p_module) {
    core::taskqueue.push(p_module);
}

void agk::flags::contains(uint16_t &flags, uint16_t check) {
    return flags & check;
}

imodule *agk::asset::find(std::string_view asset_name) {
    return agk::app.p_asset_manager_service->find(asset_name);
}

void agk::asset::load(imodule *p_asset) {
    agk::app.p_asset_manager_service->load(p_asset);
}
