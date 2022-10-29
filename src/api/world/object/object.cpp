#include "object.hpp"
#include "api/api.hpp"

void object::on_create() {
    feature::on_create();
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

object::object(material::data* data) {
    if (this->material == nullptr) {
        this->material = data;
    }
}

object::~object() {
    if (this->material != nullptr) {
        delete this->material;
        this->material = nullptr;
    }
}
