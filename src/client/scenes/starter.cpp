#include "starter.hpp"
#include "api/gpu/buffering.hpp"
#include "api/util/env.hpp"
#include "api/api.hpp"

void client::scenes::starter::on_create() {
}

void client::scenes::starter::on_destroy() {

}

void client::scenes::starter::on_event(SDL_Event &sdl_event) {
    auto camera {api::world::currentcamera()};

    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONDOWN: {

            break;
        }

        case SDL_MOUSEBUTTONUP: {

            break;
        }

        case SDL_KEYDOWN: {

            break;
        }

        case SDL_MOUSEMOTION: {
            if (api::input::pressed("mouse-right")) {

            }
            break;
        }
    }
}

void client::scenes::starter::on_update() {
}

void client::scenes::starter::on_render() {

}