#pragma once

#ifndef AGK_SCENE_H
#define AGK_SCENE_H

#include "api/agk_feature.hpp"

class agk_scene : public agk_feature {
public:
    virtual void on_start();
    virtual void on_end();

    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_render() override;
};

#endif