#ifndef AGK_API_FEATURE_H
#define AGK_API_FEATURE_H

#include <SDL2/SDL.h>
#include <iostream>

class feature {
public:
    feature();
    ~feature();

    static int32_t token;
    bool is_enable {true};
    bool is_dead {};
    int32_t id {};
    std::string tag {};

    virtual void on_create();
    virtual void on_destroy();
    virtual void on_event(SDL_Event &sdl_event);
    virtual void on_update();
    virtual void on_render();
};

#endif