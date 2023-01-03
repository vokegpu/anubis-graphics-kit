#include "gc.hpp"
#include "api/util/env.hpp"

void gc::destroy(feature *p_feature) {
    if (p_feature == nullptr) {
        return;
    }

    this->queue.push(p_feature);
    this->should_poll = true;
}

void gc::create(feature *p_feature) {
    if (p_feature == nullptr) {
        return;
    }

    this->queue.push(p_feature);
    this->should_poll = true;
}

void gc::do_update() {
    if (this->should_poll) {
        while (!this->queue.empty()) {
            feature *&p_feature {this->queue.front()};
            if (p_feature == nullptr) {
                continue;
            } else if (p_feature->is_dead) {
                p_feature->on_destroy();
                delete p_feature;
                p_feature = nullptr;
            } else {
                p_feature->on_create();
            }

            this->queue.pop();
        }

        this->should_poll = false;
    }
}