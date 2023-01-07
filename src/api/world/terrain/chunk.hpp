#ifndef AGK_API_WORLD_TERRAIN_CHUNK_H
#define AGK_API_WORLD_TERRAIN_CHUNK_H

#include "api/gpu/buffering.hpp"
#include "api/mesh/mesh.hpp"

class chunk {
protected:
    bool processed {};
public:
    buffering buffering {};
    mesh::data meshing_data {};

    bool gen_chunk(void *p_raster_terrain, int32_t min, int32_t max);
    bool is_processed();
};

#endif