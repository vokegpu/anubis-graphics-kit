#pragma once
#ifndef SCENE_CUBE_H
#define SCENE_CUBE_H

#include "api/agk_scene.hpp"

class scene_cube : public agk_scene {
public:
    uint32_t vbo, vao, ebo;

    void on_start() override;
    void on_end() override;

    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_render() override;
};

#endif