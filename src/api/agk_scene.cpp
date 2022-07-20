#include "agk_scene.hpp"

void agk_scene::on_start() {

}

void agk_scene::on_end() {

}

void agk_scene::on_event(SDL_Event &sdl_event) {
    agk_feature::on_event(sdl_event);
}

void agk_scene::on_update() {
    agk_feature::on_update();
}

void agk_scene::on_render() {
    agk_feature::on_render();
}
