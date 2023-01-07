#ifndef AGK_API_WORLD_TERRAIN_CHUNK_H
#define AGK_API_WORLD_TERRAIN_CHUNK_H

#include "api/gpu/buffering.hpp"
#include "api/mesh/mesh.hpp"

class chunk {
protected:
    bool processed {};
public:
    buffering buffering {};
    mesh::data data {};

    void gen_chunk(void *p_raster_terrain);
    bool is_processed();
};

#endif