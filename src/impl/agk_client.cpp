#include "agk_client.hpp"
#include "api/agk_util.hpp"
#include "api/agk_render.hpp"
#include "impl/scenes/scene_main.hpp"
#include <glm/gtc/matrix_transform.hpp>

agk_client::agk_client() = default;
agk_client::~agk_client() = default;

void agk_client::init() {
    agk_util::log("AGK initialising!");

    if (SDL_Init(SDL_INIT_VIDEO)) {
        agk_util::log("Could not initialise SDL2.");
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetSwapInterval(1);

    this->sdl_window = SDL_CreateWindow("Anubis Graphics Kit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->screen_width, this->screen_height,  SDL_WINDOW_OPENGL);
    this->sdl_gl_context = SDL_GL_CreateContext(this->sdl_window);

    glewExperimental = true;
    if (glewInit()) {
        agk_util::log("Could not initialise API OpenGL extensions.");
    }

    agk_util::log("AGK core initialised!");
}

void agk_client::shutdown() {
    agk_util::log("Quitting from AGK, bye-bye!");
    SDL_free(this->sdl_window);
}

void agk_client::run() {
    agk_batch3d::init();
    agk_clock::set_fps(60);
    agk_util::init();
    SDL_Event sdl_event;

    this->camera = new agk_camera();
    this->load_scene(new scene_main());

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

void agk_client::on_event_segment(SDL_Event &sdl_event) {
    agk_util::keyboard(sdl_event);

    switch (sdl_event.type) {
        case SDL_QUIT: {
            this->running = false;
            break;
        }

        case SDL_WINDOWEVENT: {
            switch (sdl_event.window.type) {
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    this->update_window_size(sdl_event.window.data1, sdl_event.window.data2);
                    break;
                }
            }

            break;
        }
    }

    if (this->scene != nullptr) {
        this->scene->on_event(sdl_event);
    }
}

void agk_client::on_update_segment() {
    if (this->scene != nullptr) {
        this->scene->on_update();
    }
}

void agk_client::on_render_segment() {
    glViewport(0, 0, static_cast<int32_t>(this->screen_width), static_cast<int32_t>(this->screen_height));
    this->glm_matrix_perspective = glm::perspective(glm::radians(this->camera->fov), (this->screen_width / this->screen_height), 0.1f, 100.0f);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if (this->scene != nullptr) {
        this->scene->on_render();
    }
}

void agk_client::load_scene(agk_scene* raw_scene) {
    if (this->scene == raw_scene) {
        return;
    }

    if (this->scene != nullptr && raw_scene == nullptr) {
        this->scene->on_end();
        delete this->scene;
        this->scene = nullptr;
        return;
    }

    if (this->scene == nullptr && raw_scene != nullptr) {
        this->scene = raw_scene;
        this->scene->on_start();
        return;
    }

    if (this->scene != nullptr && raw_scene != nullptr) {
        this->scene->on_end();
        delete this->scene;
        this->scene = raw_scene;
        this->scene->on_start();
    }
}

float agk_client::get_screen_width() {
    return this->screen_width;
}

float agk_client::get_screen_height() {
    return this->screen_height;
}

SDL_Window *agk_client::get_sdl_win() {
    return this->sdl_window;
}

glm::mat4 agk_client::matrix_perspective() {
    return this->glm_matrix_perspective;
}

void agk_client::update_window_size(uint32_t w, uint32_t h) {
    this->screen_width = static_cast<float>(w);
    this->screen_height = static_cast<float>(h);
}
