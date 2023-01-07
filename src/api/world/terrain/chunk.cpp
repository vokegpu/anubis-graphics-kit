#include "chunk.hpp"

void chunk::gen_chunk(void *p_raster_terrain) {
    this->data.faces = 0;
    auto &v {this->data.get_float_list(mesh::type::vertex)};
    v.clear();

    for
}

bool chunk::is_processed() {
    return false;
}
