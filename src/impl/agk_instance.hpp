#pragma once

#ifndef AGK_INSTANCE_H
#define AGK_INSTANCE_H

#include <SDL2/SDL.h>
#include <GL/glew.h>

/**
 * The core of Anubis Graphics Kit.
 */
class agk_instance {
protected:
    SDL_Window* sdl_window;
    SDL_GLContext sdl_gl_context;

    bool running = true;

    float screen_width;
    float screen_height;
public:
    agk_instance();
    ~agk_instance();

    void init();
    void shutdown();
    void run();

    void on_event_segment(SDL_Event &sdl_event);
    void on_update_segment();
    void on_render_segment();
};

#endif