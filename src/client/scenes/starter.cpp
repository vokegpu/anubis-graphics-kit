#include "starter.hpp"
#include "api/render/buffer_builder.hpp"
#include "api/util/env.hpp"
#include "api/api.hpp"

void client::scenes::starter::on_create() {
	float vert[9] {
		-1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

    api::gc::create(this->model = new buffer_builder());
	this->model->invoke();
	this->model->bind();
	this->model->send_data(sizeof(float) * 9, vert, GL_STATIC_DRAW);
	this->model->vert_amount = 3;
	this->model->revoke();
}

void client::scenes::starter::on_destroy() {

}

void client::scenes::starter::on_event(SDL_Event &sdl_event) {

}

void client::scenes::starter::on_update() {

}

void client::scenes::starter::on_render() {
    this->model->on_render();
}