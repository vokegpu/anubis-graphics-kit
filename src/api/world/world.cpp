#include "world.hpp"
#include "api/api.hpp"
#include "memory.hpp"

void world::registry_wf(world_feature *wf) {
	if (wf == nullptr) {
		return;
	}

	wf->id = this->++wf_token_id;
	api::gc::create(wf);

	this->registered_wf_map[wf->id] = wf;
	this->wf_list.push_back(wf);
}

void world::refresh_wf_renderable() {
	this->wf_draw_list.clear();

	for (world_feature *&wf : this->wf_list) {
		if (wf != nullptr && wf->visible == enums::state::enable) {
			this->wf_draw_list.push_back(wf);
		}
	}
}

void world::append_process(world_feature *wf) {
	if (memory::check(wf->id) == nullptr) {
		memory::emmite(wf->id, wf);
		this->wf_process_queue.push(wf);
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
		auto &wf {this->wf_process_queue.front()};
		if (wf != nullptr) wf->on_update();
		this->wf_process_queue.pop();
	}
}

void world::on_render() {
	for (world_feature *&wf : this->wf_draw_list) {
		wf->on_render();
	}
}