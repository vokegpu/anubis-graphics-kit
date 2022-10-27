#include "starter.hpp"
#include "api/gpu/buffer_builder.hpp"
#include "api/util/env.hpp"
#include "api/api.hpp"

void client::scenes::starter::on_create() {
    api::mesh::model("cat", mesh::format::obj, "./data/models/cube.obj");

    auto cat_object {new object {}};
    cat_object->scale = glm::vec3(1, 1, 1);
    api::mesh::assign(cat_object, "cat");

    api::world::create(cat_object);
    api::world::current().camera_movement = true;
    api::world::camera3d().enabled = false;
}

void client::scenes::starter::on_destroy() {

}

void client::scenes::starter::on_event(SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            if (!api::world::camera3d().enabled && api::input::pressed("mouse-left")) {
                api::world::camera3d().enabled = true;
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            if (api::world::camera3d().enabled && !api::input::pressed("mouse-left")) {
                api::world::camera3d().enabled = false;
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
            break;
        }
    }
}

void client::scenes::starter::on_update() {

}

void client::scenes::starter::on_render() {

}