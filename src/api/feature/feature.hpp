#ifndef AGK_API_FEATURE_H
#define AGK_API_FEATURE_H

#include <SDL2/SDL.h>

class feature {
public:
    bool is_enable {true};
    bool is_dead {};

    virtual void on_create();
    virtual void on_destroy();
    virtual void on_event(SDL_Event &sdl_event);
    virtual void on_update();
    virtual void on_render();
};

#endif