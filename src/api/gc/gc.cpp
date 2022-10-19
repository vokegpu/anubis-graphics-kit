#include "gc.hpp"

void gc::destroy(feature* target) {
	gc::queue_undestroyed.emplace_back(target);
	this->should_poll_undestroyed = true;
}

void gc::on_create(feature* target) {
	gc::queue_uncreated.emplace_back(target);
	this->should_poll_uncreated = true;
}

void gc::do_update() {
	if (this->should_poll_uncreated) {
		while (!gc::queue_uncreated.empty()) {
			auto &target {gc::queue_uncreated.front()};
			if (target != nullptr) target->on_create();
			gc::queue_uncreated.pop_front()
		}

		this->should_poll_uncreated = false;
	}

	if (this->should_poll_undestroyed) {
		while (!gc::queue_undestroyed.empty()) {
			auto &target {gc::queue_undestroyed.front()};
			if (target != nullptr) {
				target->on_destroy();
				delete target;
				target = nullptr;
			}
			gc::queue_undestroyed.pop_front()
		}

		this->should_poll_undestroyed = false;
	}
}