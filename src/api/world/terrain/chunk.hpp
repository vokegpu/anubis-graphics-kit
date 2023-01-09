#ifndef AGK_API_WORLD_TERRAIN_CHUNK_H
#define AGK_API_WORLD_TERRAIN_CHUNK_H

#include "api/gpu/buffering.hpp"
#include "api/mesh/mesh.hpp"
#include "api/world/world_feature.hpp"
#include "api/util/math.hpp"

class chunk : public world_feature {
protected:
    bool buffer_processed {};
    bool mesh_processed {};
public:
    buffering buffering {};
    mesh::data meshing_data {};

    bool gen_chunk(void *p_raster_terrain, uint32_t width, uint32_t height);
    void set_buffer_processed();

    bool is_buffer_processed();
    bool is_mesh_processed();
};

#endif