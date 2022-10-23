#include "world.hpp"

void world::on_create() {
    feature::on_create();
}

void world::on_destroy() {
    feature::on_destroy();
}

void world::on_event(SDL_Event &sdl_event) {
    feature::on_event(sdl_event);
}

void world::on_update() {
    feature::on_update();
}

void world::on_render() {
    feature::on_render();
}
