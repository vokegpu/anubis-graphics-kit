#include "object.hpp"
#include "api/api.hpp"

void object::set_model(std::string_view tag) {
    this->model_id = api::app.world_client.get_model_id_by_tag(tag);
}

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
