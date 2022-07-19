#pragma once

#ifndef AGK_FEATURE_H
#define AGK_FEATURE_H

#include <iostream>
#include <string>
#include <SDL2/SDL.h>

/**
 * Feature to registry objects into Anubis Graphics Kit.
 */
class agk_feature {
protected:
    std::string tag;
public:
    agk_feature();
    ~agk_feature();

    void set_tag(const std::string &str);
    std::string get_tag();

    virtual void on_event(SDL_Event &sdl_event);
    virtual void on_update();
    virtual void on_render();
};

#endif