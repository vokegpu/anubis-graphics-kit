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

    bool running = true;

    float screen_width;
    float screen_height;
public:
    agk_core();
    ~agk_core();

    void init();
    void shutdown();
    void run();

    void on_event_segment(SDL_Event &sdl_event);
    void on_update_segment();
    void on_render_segment();
};

#endif