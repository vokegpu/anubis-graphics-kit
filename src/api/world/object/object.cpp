#include "object.hpp"
#include "api/api.hpp"

void object::on_create() {
    feature::on_create();
    this->material = &api::world::render().undefined_material;
}

void object::on_destroy() {
    feature::on_destroy();
}

void object::on_event(SDL_Event &sdl_event) {
    feature::on_event(sdl_event);
}

void object::on_update() {
    feature::on_update();
}

void object::on_render() {
    feature::on_render();
}
