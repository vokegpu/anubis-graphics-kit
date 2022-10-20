#include "starter.hpp"
#include "api/render/buffer_builder.hpp"
#include "api/util/runtime.hpp"

static buffer_builder model {};

void client::scenes::starter::on_create() {
	float vert[9] {
		-1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

	model.invoke();
	model.bind();
	model.send_data(sizeof(float) * 9, vert, GL_STATIC_DRAW);
	model.vert_amount = 3;
	model.revoke();
}

void client::scenes::starter::on_destroy() {

}

void client::scenes::starter::on_event(SDL_Event &sdl_event) {

}

void client::scenes::starter::on_update() {

}

void client::scenes::starter::on_render() {
	model.draw();
}