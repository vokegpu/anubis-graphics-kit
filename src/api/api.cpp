#include "api.hpp"
#include "api/util/env.hpp"
#include <GL/glew.h>

core api::app {};

void api::mainloop(feature* initial_scene) {
	util::log("initialising");

	if (SDL_Init(SDL_INIT_VIDEO)) {
		util::log("failed to init SDL");
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	api::app.root = SDL_CreateWindow("Anubis Graphics Kit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, api::app.screen_width, api::app.screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	util::log("window released");

	glewExperimental = true;
	SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(api::app.root)};
	SDL_GL_SetSwapInterval(1);
	util::log("OpenGL 4 context created");

	util::timing reduce_cpu_ticks_timing {};
	util::timing counter_fps_timing {};

	uint64_t cpu_ticks_interval {api::app.fps};
	uint64_t fps_interval {1000};
	uint64_t ticked_frames {};
	SDL_Event sdl_event {};

	api::app.mainloop = true;
	api::scene::load(initial_scene);
    api::gc::create(&api::app.world_render_manager);
    api::gc::create(&api::app.world_client);

	while (api::app.mainloop) {
		if (util::resetifreach(reduce_cpu_ticks_timing, cpu_ticks_interval)) {
			while (SDL_PollEvent(&sdl_event)) {
				switch (sdl_event.type) {
					case SDL_QUIT: {
						api::app.mainloop = false;
						break;
					}

					default: {
						if (api::app.current_scene != nullptr) api::app.current_scene->on_event(sdl_event);
						break;
					}
				}
			}

			if (util::resetifreach(reduce_cpu_ticks_timing, fps_interval)) {
				api::app.display_fps = ticked_frames;
				ticked_frames = 0;
			}

			if (api::app.current_scene != nullptr) {
				api::app.current_scene->on_update();
			}

			api::app.garbage_collector.do_update();

            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			if (api::app.current_scene != nullptr) {
				api::app.current_scene->on_render();
			}

			ticked_frames++;
			SDL_GL_SwapWindow(api::app.root);
		}
	}

    api::app.shader_manger.quit();
}

void api::scene::load(feature* scene) {
	auto &current_scene {api::scene::current()};

	// if you think it is bad, you should code more.

	if (scene == nullptr && current_scene != nullptr) {
		api::gc::destroy(current_scene);
		api::app.current_scene = nullptr;
		return;
	}

	if (scene != nullptr && current_scene != nullptr && current_scene == scene) {
		api::gc::destroy(scene);
		return;
	}

	if (scene != nullptr && current_scene != nullptr && current_scene != scene) {
		api::gc::destroy(current_scene);
		api::gc::create(scene);
		api::app.current_scene = scene;
		return;
	}

	if (scene != nullptr && current_scene == nullptr) {
		api::gc::create(scene);
		api::app.current_scene = scene;
	}
}

feature* &api::scene::current() {
	return api::app.current_scene;
}

void api::gc::destroy(feature* target) {
	api::app.garbage_collector.destroy(target);
}

void api::gc::create(feature* target) {
	api::app.garbage_collector.create(target);
}

bool api::shading::createprogram(std::string_view name, ::shading::program &program, const std::vector<::shading::resource> &resources) {
    return api::app.shader_manger.create_shading_program(name, program, resources);
}
