#ifndef AGK_EVENT_H
#define AGK_EVENT_H

#include <SDL2/SDL.h>

namespace event {
    enum type {
        world_refresh_environment,
        world_changed_priority,
        world_refresh_environment_lighting
    };

    void dispatch(SDL_Event &sdl_event, const event::type &type);
};

#endif