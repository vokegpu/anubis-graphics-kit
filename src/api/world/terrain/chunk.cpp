#include "chunk.hpp"
#include "api/api.hpp"

bool chunk::gen_chunk(void *p_raster_terrain, int32_t min, int32_t max) {
    if (this->mesh_processed) {
        return false;
    }

    this->meshing_data.faces = 0;
    this->scale += 10;

    glm::vec3 v1 {};
    glm::vec3 v2 {};
    glm::vec3 v3 {};

    this->mesh_processed = true;
    return true;
}

bool chunk::is_mesh_processed() {
    return this->mesh_processed;
}

bool chunk::is_buffer_processed() {
    return this->buffer_processed;
}

void chunk::set_buffer_processed() {
    this->buffer_processed = true;
}
