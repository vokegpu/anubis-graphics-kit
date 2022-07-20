#pragma once

#ifndef AGK_H
#define AGK_H

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "api/agk_camera.hpp"
#include "api/agk_scene.hpp"

/**
 * The core of Anubis Graphics Kit.
 **/
class agk {
protected:
    SDL_Window* sdl_window;
    SDL_GLContext sdl_gl_context;

    float screen_width;
    float screen_height;

    bool running = true;
public:
    agk_camera* camera = nullptr;
    agk_scene* scene = nullptr;

    agk();
    ~agk();

    void init();
    void shutdown();
    void run();

    void on_event_segment(SDL_Event &sdl_event);
    void on_update_segment();
    void on_render_segment();

    void load_scene(agk_scene* raw_scene);
};

#endif