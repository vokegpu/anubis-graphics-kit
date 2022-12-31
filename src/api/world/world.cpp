#include "world.hpp"
#include "api/api.hpp"
#include "api/gc/memory.hpp"
#include "api/world/environment/entity.hpp"
#include "api/world/model/model.hpp"
#include "api/world/environment/object.hpp"

void world::registry_wf(world_feature *p_world_feature) {
	if (p_world_feature == nullptr) {
		return;
	}

    p_world_feature->id = ++this->wf_token_id;
	api::gc::create(p_world_feature);

	this->registered_wf_map[p_world_feature->id] = p_world_feature;
	this->wf_list.push_back(p_world_feature);
}

void world::refresh_wf_renderer() {
	this->wf_draw_list.clear();

	for (world_feature *&p_world_feature : this->wf_list) {
		if (p_world_feature != nullptr && p_world_feature->visible == enums::state::enable) {
			this->wf_draw_list.push_back(p_world_feature);
		}
	}
}

void world::append_process(world_feature *p_world_feature) {
	if (memory::check(p_world_feature->id) == nullptr) {
		memory::emmite(p_world_feature->id, p_world_feature);
		this->wf_process_queue.push(p_world_feature);
	}
}

void world::on_create() {

}

void world::on_destroy() {

}

void world::on_event(SDL_Event &sdl_event) {

}

void world::on_update() {
	while (!this->wf_draw_list.empty()) {
		auto &p_world_feature {this->wf_process_queue.front()};
		if (p_world_feature != nullptr) p_world_feature->on_update();
		this->wf_process_queue.pop();
	}
}

void world::on_render() {
    api::world::camera();
    glm::mat4 mvp {};

    entity *p_entity {};
    model *p_model {};
    object *p_object {};

	for (world_feature *&p_world_feature : this->wf_draw_list) {
        if (p_world_feature == nullptr) {
            continue;
        }

        switch (p_world_feature->type) {
            case enums::type::entity: {
                p_entity = (entity*) p_world_feature;
                p_model = p_entity->p_model;
                break;
            }

            case enums::type::object: {
                p_object = (object*) p_world_feature;
                p_model = p_object->p_model;
                break;
            }

            default: {
                break;
            }
        }

        if (p_model == nullptr) {
            continue;
        }

        p_world_feature->on_render();
    }
}