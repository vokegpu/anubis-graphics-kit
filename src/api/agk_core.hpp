#pragma once
#ifndef AGK_CORE_H
#define AGK_CORE_H

#include <SDL2/SDL.h>
#include <gl/glew.h>

/**
 * The core of Anubis Graphics Kit.
 */
class agk_core {
protected:
    SDL_Window* sdl_window;
    SDL_GLContext sdl_gl_context;
public:
    agk_core();
    ~agk_core();

    void init();
    void shutdown();
    void run();
};

#endif