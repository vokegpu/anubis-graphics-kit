#include "starter.hpp"
#include "api/gpu/buffer_builder.hpp"
#include "api/util/env.hpp"
#include "api/api.hpp"

void client::scenes::starter::on_create() {
    api::mesh::model("cat", mesh::format::obj, "./data/models/Alien Animal.obj");

    auto solid {new material::solid {material::composition::opaque}};
    auto cat_object {new object {solid}};
    cat_object->scale = glm::vec3(0.5f, 0.5f, 0.5f);
    solid->color[0] = 1.022f;
    solid->color[1] = 0.782f;
    solid->color[2] = 0.334f;

    auto ct_object {new object {solid}};
    ct_object->scale = glm::vec3(0.5f, 0.5f, 0.5f);
    ct_object->position = glm::vec3(0, 20, 0);

    auto mate {new material::light {material::composition::light}};
    this->obj = new object {mate};
    mate->intensity[0] = 0.4f;
    mate->intensity[1] = 0.4f;
    mate->intensity[2] = 0.4f;

    api::mesh::assign(cat_object, "cat");
    api::mesh::assign(ct_object, "cat");

    api::world::create(cat_object);
    api::world::create(ct_object);
    api::world::create(this->obj);

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
    if (this->obj != nullptr && api::input::pressed("space")) this->obj->position = api::world::camera3d().position;
}

void client::scenes::starter::on_render() {

}