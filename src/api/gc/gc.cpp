#include "gc.hpp"

std::queue<feature*> gc::queue_uncreated {};
std::queue<feature*> gc::queue_undead {};

void gc::destroy(feature* target) {
	gc::queue_undead.push(target);
	this->should_poll_undead = true;
}

void gc::create(feature* target) {
	gc::queue_uncreated.push(target);
	this->should_poll_uncreated = true;
}

void gc::do_update() {
	if (this->should_poll_uncreated) {
		while (!gc::queue_uncreated.empty()) {
			feature* &target {gc::queue_uncreated.front()};
			if (target != nullptr) target->on_create();
			gc::queue_uncreated.pop();
		}

		this->should_poll_uncreated = false;
	}

	if (this->should_poll_undead) {
		while (!gc::queue_undead.empty()) {
			feature* &target {gc::queue_undead.front()};
			if (target != nullptr) {
				target->on_destroy();
				delete target;
				target = nullptr;
			}
			gc::queue_undead.pop();
		}

		this->should_poll_undead = false;
	}
}