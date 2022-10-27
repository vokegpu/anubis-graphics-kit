#include "entity.hpp"
#include "api/api.hpp"

void entity::on_create() {

}

void entity::on_destroy() {

}

void entity::on_event(SDL_Event &sdl_event) {

}

void entity::on_update() {
	this->position += this->velocity * api::dt;
}

void entity::on_render() {

}