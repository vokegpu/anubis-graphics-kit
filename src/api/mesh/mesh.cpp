#include "mesh.hpp"
#include <string.h>

bool mesh::packed_vertex::operator < (const mesh::packed_vertex p) const {
    return memcmp((void*) this, (void*) &p, sizeof(mesh::packed_vertex)) > 0;
}