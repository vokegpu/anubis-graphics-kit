#ifndef AGK_API_EVENT_H
#define AGK_API_EVENT_H

#include <SDL2/SDL.h>

struct event {
    static int32_t constexpr WORLD_REFRESH_DRAW {1};
    static int32_t constexpr WORLD_REFRESH_LOW_PRIORITY {2};
    static int32_t constexpr WORLD_REFRESH_HIGH_PRIORITY {3};

    static void dispatch(SDL_Event &sdl_event, int32_t sdl_event_type);
};

#endif