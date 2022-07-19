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
public:
    void init();
    void shutdown();
    void run();
};

#endif