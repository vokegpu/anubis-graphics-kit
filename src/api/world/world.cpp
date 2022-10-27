#include "world.hpp"
#include "api/api.hpp"
#include <glm/glm.hpp>
#include "api/util/env.hpp"

void world::on_create() {
    feature::on_create();
    this->loaded_entity_list.push_back((this->player = new entity()));
}

void world::on_destroy() {
    feature::on_destroy();
}

void world::on_event(SDL_Event &sdl_event) {
    feature::on_event(sdl_event);
}

void world::on_update() {
    feature::on_update();

    if (this->camera_movement) {
        this->do_camera_movement();
    }

    for (entity* &entities : this->loaded_entity_list) {
        entities->on_update();
    }
}

void world::on_render() {
    feature::on_render();
}

void world::do_camera_movement() {
    glm::vec3 input {};

    if (api::input::pressed("w")) {
        input.z = 1;
    }

    if (api::input::pressed("s")) {
        input.z = -1;   
    }

    if (api::input::pressed("a")) {
        input.x = 1;
    }

    if (api::input::pressed("d")) {
        input.x = -1;
    }

    float speed {4.2987f};
    float f {this->player->yaw};
    float x {glm::cos(glm::radians(f))};
    float z {glm::sin(glm::radians(f))};

    api::app.world_camera3d.position = this->player->position;

    this->player->yaw = api::app.world_camera3d.yaw;
    this->player->pitch = api::app.world_camera3d.pitch;
    this->player->velocity.x = input.z * speed * x + input.x * speed * z;
    this->player->velocity.z = input.z * speed * z - input.x * speed * x;
}