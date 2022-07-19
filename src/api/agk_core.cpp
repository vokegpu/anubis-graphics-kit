#include "agk_core.hpp"
#include "agk_util.hpp"
#include "agk_renderer_manager.hpp"

agk_core::agk_core() {

}

agk_core::~agk_core() {

}

void agk_core::init() {
    util::log("AGK initialising!");

    if (SDL_Init(SDL_INIT_VIDEO)) {
        util::log("Could not initialise SDL2.");
        return;
    }

    this->sdl_window = SDL_CreateWindow("Anubis Graphics Kit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 800, SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    this->sdl_gl_context = SDL_GL_CreateContext(this->sdl_window);

    glewExperimental = true;
    if (glewInit()) {
        util::log("Could not initialise API OpenGL extensions.");
    }

    util::log("AGK core initialised!");
}

void agk_core::shutdown() {
    util::log("Quitting from AGK, bye-bye!");
    SDL_free(this->sdl_window);
}

void agk_core::run() {
    agk_fx_manager::init();
    agk_clock::set_fps(60);

    SDL_Event sdl_event;

    /*
     * The game main loop.
     */
    while (this->running) {
        // Run the clock.
        agk_clock::run();

        // If elapsed ticks is greater than interval fps we need to sync.
        if (agk_clock::ticks_running_asynchronous()) {
            agk_clock::sync_running_ticks();

            while (SDL_PollEvent(&sdl_event)) {
                this->on_event_segment(sdl_event);
            }

            this->on_update_segment();
            this->on_render_segment();

            // Check the frame rate of current loop.
            agk_clock::check_frame_rate();

            // Swap buffers.
            SDL_GL_SwapWindow(this->sdl_window);
        }
    }
}

void agk_core::on_event_segment(SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_QUIT: {
            this->running = false;
            break;
        }

        case SDL_WINDOWEVENT: {
            switch (sdl_event.window.type) {
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    this->screen_width = static_cast<float>(sdl_event.window.data1);
                    this->screen_height = static_cast<float>(sdl_event.window.data2);

                    glViewport(0, static_cast<GLint>(sdl_event.window.data1), static_cast<GLint>(sdl_event.window.data1), 0);
                    break;
                }
            }

            break;
        }
    }
}

void agk_core::on_update_segment() {

}

void agk_core::on_render_segment() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}
