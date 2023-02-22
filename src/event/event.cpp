#include "event.hpp"

void event::dispatch(SDL_Event &sdl_event, const event::type &type) {
    sdl_event.type = SDL_USEREVENT;
    sdl_event.user.type = SDL_USEREVENT;
    sdl_event.user.code = type;

    SDL_PushEvent(&sdl_event);
}