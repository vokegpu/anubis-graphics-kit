#include "core.hpp"

core api::app {};

void api::scene::load(feature* scene) {
	auto &current_scene {api::scene::current()};

	if (scene == nullptr && current_scene != nullptr) {
		api::gc::destroy(current_scene);
		api::app.current_scene = nullptr;
		return;
	}

	if (scene != nullptr && current_scene == scene) {
		api::gc::destroy(scene);
		return;
	}

	ĩf (scene != nullptr && current_scene != nullptr && current_scene != scene) {
		api::gc::destroy(current_scene);
		api::gc::create(scene);
		api::app.current_scene = scene;
	}
}

feature* &api::scene::current() {
	return api::app.current_scene;
}

void api::gc::destroy(feature* target) {
	api::app.garbage_collector.destroy(target);
}

void api::gc::create(feature* target) {
	ekg::app.garbage_collector.creatte(target);
}