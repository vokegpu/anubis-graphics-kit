#include "starter.hpp"
#include "api/gpu/buffering.hpp"
#include "api/util/env.hpp"
#include "api/api.hpp"

void client::scenes::starter::on_create() {
}

void client::scenes::starter::on_destroy() {

}

void client::scenes::starter::on_event(SDL_Event &sdl_event) {
    auto &camera {api::world::camera3d()};

    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            if (!camera.enabled && api::input::pressed("mouse-left")) {
                camera.enabled = true;
            }
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            if (camera.enabled && !api::input::pressed("mouse-left")) {
                camera.enabled = false;
            }

            break;
        }

        case SDL_KEYDOWN: {
            this->solid->rough += 0.05f * api::input::pressed("v");
            this->solid->rough -= 0.05f * api::input::pressed("c");
            break;
        }

        case SDL_MOUSEMOTION: {
            if (api::input::pressed("mouse-right")) {
                api::world::current().player->velocity.x += static_cast<float>(sdl_event.motion.xrel);
                api::world::current().player->velocity.z += static_cast<float>(sdl_event.motion.yrel);
            }
            break;
        }
    }
}

void client::scenes::starter::on_update() {
    if (this->obj != nullptr && api::input::pressed("space")) this->obj->position = api::world::camera3d().position;
    if (this->obj != nullptr && api::input::pressed("x")) this->l2->position = api::world::camera3d().position;
}

void client::scenes::starter::on_render() {

}