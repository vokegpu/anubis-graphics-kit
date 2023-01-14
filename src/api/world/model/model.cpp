#include "model.hpp"

void model::on_create() {
    feature::on_create();
}

void model::on_destroy() {
    feature::on_destroy();
    this->buffer.free_buffers();
}
