#include "camera_manager.hpp"
#include "api/api.hpp"

void camera_manager::on_create() {
    feature::on_create();
    this->p_camera_linked = api::world::currentcamera();
}

void camera_manager::on_destroy() {
    feature::on_destroy();
}

void camera_manager::on_event(SDL_Event &sdl_event) {
    feature::on_event(sdl_event);

    if (this->camera_rotate == 1 && api::input::pressed("mouse-1")) {

    }
}

void camera_manager::on_update() {
    feature::on_update();
}
