#pragma once

#ifndef AGK_CLIENT_H
#define AGK_CLIENT_H

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "api/agk_camera.hpp"
#include "api/agk_scene.hpp"

/**
 * The core of Anubis Graphics Kit.
 **/
class agk_client {
protected:
    SDL_Window* sdl_window{};
    SDL_GLContext sdl_gl_context{};

    float screen_width = 800;
    float screen_height = 600;

    bool running = true;
    glm::mat4 glm_matrix_perspective{};

    void update_window_size(uint32_t w, uint32_t hj);
public:
    agk_camera* camera = nullptr;
    agk_scene* scene = nullptr;

    SDL_Window* get_sdl_win();
    glm::mat4 matrix_perspective();

    float get_screen_width();
    float get_screen_height();

    agk_client();
    ~agk_client();

    void init();
    void shutdown();
    void run();

    void on_event_segment(SDL_Event &sdl_event);
    void on_update_segment();
    void on_render_segment();

    void load_scene(agk_scene* raw_scene);
};

#endif