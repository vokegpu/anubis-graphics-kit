#include "starter.hpp"
#include "api/renderer/buffer_builder.hpp"
#include "api/util/env.hpp"
#include "api/api.hpp"

void client::scenes::starter::on_create() {
	float vert[9] {
		-1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

    auto model {api::world::render().gen_model("hi")};
    auto cat_object {new object {}};
    cat_object->model_id = model->id;
    cat_object->scale = glm::vec3(0.25f, 0.25f, 0.25f);

    mesh::data cat {mesh::format::obj};
    api::mesh::load(cat, "./data/models/cat.obj");

    api::world::create(cat_object);
    api::mesh::compile(cat, model);
    api::world::current().camera_movement = true;
}

void client::scenes::starter::on_destroy() {

}

void client::scenes::starter::on_event(SDL_Event &sdl_event) {

}

void client::scenes::starter::on_update() {

}

void client::scenes::starter::on_render() {

}