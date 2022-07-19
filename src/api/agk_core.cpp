#include "agk_core.hpp"
#include "agk_util.hpp"

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

}