#include "event.hpp"
#include "api/util/env.hpp"

void event::dispatch(SDL_Event &sdl_event, int32_t sdl_event_type) {
    sdl_event.type = SDL_USEREVENT;
    sdl_event.user.type = SDL_USEREVENT;
    sdl_event.user.code = sdl_event_type;
    SDL_PushEvent(&sdl_event);
}
