#include <iostream>
#include "api/core/core.hpp"
#include "api/util/runtime.hpp"
#include <GL/glew.h>

int32_t main(int32_t, char**) {
	util::log("initialising");

	if (SDL_Init(SDL_INIT_VIDEO)) {
		util::log("failed to init SDL");
		return 1;
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
	util::timing counter_fps_timning {};

	uint64_t cpu_ticks_interval {api::app.fps};
	uint64_t fps_interval {1000};
	uint64_t ticked_frames {};
	SDL_Event sdl_event {};

	api::app.mainloop = true;
	while (api::app.mainloop) {
		if (util::reset_if_reach(reduce_cpu_ticks_timing, cpu_ticks_interval)) {
			while (SDL_PollEvent(&sdl_event)) {
				switch (sdl_event.type) {
					case SDL_QUIT: {
						api::app.mainloop = false;
						break;
					}
				}
			}

			if (util::reset_if_reach(reduce_cpu_ticks_timing, fps_interval)) {
				api::app.display_fps = ticked_frames;
				ticked_frames = 0;
			}

			glViewport(0, 0, api::app.screen_width, api::app.screen_height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			ticked_frames++;
			SDL_GL_SwapWindow(api::app.root);
		}
	}

	return 0;
}