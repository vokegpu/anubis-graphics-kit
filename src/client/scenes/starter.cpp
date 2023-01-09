#include "starter.hpp"
#include "api/gpu/buffering.hpp"
#include "api/util/env.hpp"
#include "api/api.hpp"
#include "api/world/environment/object.hpp"
#include "api/world/environment/light.hpp"

void client::scenes::starter::on_create() {
    model *p_model_dino {api::world::create("Dinossaur", "./data/models/Dinossaur.stl", mesh::format::stl)};
    this->p_object_dino = new object(p_model_dino);
    material *p_material {new material(enums::material::metal)};

    p_material->set_color({1.0f, 1.0f, 0.0f});
    this->p_object_dino->p_material = p_material;
    this->p_object_dino->position.y = 30;
    this->p_object_dino->scale = {0.2f, 0.2f, 0.2f};

    this->p_light_spot = new light(nullptr);
    this->p_light_spot->intensity = {50, 50, 50};
    this->p_light_spot->position = {0, 15, 0};
    this->p_light_spot->update();

    api::world::create(this->p_object_dino);
    api::world::create(this->p_light_spot);

    this->p_camera_manager->set_editor_enabled(true);
    this->p_camera_manager->set_movement_enabled(true);
    this->p_camera_manager->set_rotation_enabled(true);
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
    if (api::input::pressed("mouse-2")) {
        this->p_light_spot->position = api::world::currentcamera()->position;
        this->p_light_spot->update();
    }

    if (api::input::pressed("h")) {
        this->p_object_dino->position = api::world::currentcamera()->position;
    }

    if (api::input::pressed("x")) {
        this->p_light_spot->intensity++;
        this->p_light_spot->intensity++;
        this->p_light_spot->intensity++;
        this->p_light_spot->intensity++;
        this->p_light_spot->intensity++;
        this->p_light_spot->intensity++;
        this->p_light_spot->intensity++;
        this->p_light_spot->update();
    }

    if (api::input::pressed("z")) {
        this->p_light_spot->intensity--;
        this->p_light_spot->intensity--;
        this->p_light_spot->intensity--;
        this->p_light_spot->intensity--;
        this->p_light_spot->intensity--;
        this->p_light_spot->intensity--;

        this->p_light_spot->update();
    }
}

void client::scenes::starter::on_render() {

}