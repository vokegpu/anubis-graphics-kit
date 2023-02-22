#include "event.hpp"

void event::dispatch(SDL_Event &sdl_event, Sint32 sdl_event_type) {
    sdl_event.type = SDL_USEREVENT;
    sdl_event.user.type = SDL_USEREVENT;
    sdl_event.user.code = sdl_event_type;

    SDL_PushEvent(&sdl_event);
}