#include "gc.hpp"
#include "api/util/env.hpp"

std::queue<feature*> gc::queue_uncreated {};
std::queue<feature*> gc::queue_undead {};

void gc::destroy(feature *p_feature) {
    if (p_feature == nullptr) {
        return;
    }

    gc::queue_undead.push(p_feature);
    this->should_poll_undead = true;
}

void gc::create(feature *p_feature) {
    if (p_feature == nullptr) {
        return;
    }

    gc::queue_uncreated.push(p_feature);
    this->should_poll_uncreated = true;
}

void gc::do_update() {
    if (this->should_poll_uncreated) {
        while (!gc::queue_uncreated.empty()) {
            auto &p_feature {gc::queue_uncreated.front()};
            if (p_feature != nullptr) p_feature->on_create();
            gc::queue_uncreated.pop();
        }

        this->should_poll_uncreated = false;
    }

    if (this->should_poll_undead) {
        while (!gc::queue_undead.empty()) {
            auto &target {gc::queue_undead.front()};
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