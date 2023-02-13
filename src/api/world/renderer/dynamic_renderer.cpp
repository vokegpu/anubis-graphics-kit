#include "dynamic_renderer.hpp"

bool dynamic_geometry_instanced_renderer::find(int32_t id, buffering &buffer) {
    return false;
}

buffering &dynamic_geometry_instanced_renderer::registry(int32_t id) {
    auto &buffer {this->buffer_map[id]};
    buffer.type = buffering::type::instanced;
    return buffer;
}

void dynamic_geometry_instanced_renderer::unregister(int32_t id) {
    if (this->buffer_map.count(id)) {
        this->buffer_map[id].free_buffers();
        this->buffer_map.erase(id);
    }
}
