#include "chunk.hpp"
#include "api/api.hpp"
#include "GL/glew.h"

chunk::chunk() {
}

chunk::~chunk() {
}

void chunk::on_create() {
    feature::on_create();
    this->visible = enums::state::enable;
    this->processed = true;
}

void chunk::on_destroy() {
    feature::on_destroy();
    this->buffer.free_buffers();
}

bool chunk::is_processed() {
    return this->processed;
}