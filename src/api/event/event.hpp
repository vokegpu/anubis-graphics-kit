#ifndef AGK_API_EVENT_H
#define AGK_API_EVENT_H

#include <SDL2/SDL.h>

struct event {
    static Sint32 constexpr WORLD_REFRESH_DRAW {1};
    static Sint32 constexpr WORLD_REFRESH_PRIORITY {2};

    static void dispatch(SDL_Event &sdl_event, Sint32 sdl_event_type);
};

#endif