#include "chunk.hpp"

chunk::chunk() {
}

chunk::~chunk() {
}

void chunk::on_create() {
    this->visible = enums::state::enable;
    this->processed = true;
}

void chunk::on_destroy() {
    this->buffer.delete_buffers();
}

bool chunk::is_processed() {
    return this->processed;
}