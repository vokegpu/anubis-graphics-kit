#include "world_render.hpp"

void world_render::on_create() {
    feature::on_create();
}

void world_render::on_destroy() {
    feature::on_destroy();
}

void world_render::on_event(SDL_Event &sdl_event) {
    feature::on_event(sdl_event);
}

void world_render::on_update() {
    feature::on_update();
}

void world_render::on_render() {
    feature::on_render();
}
