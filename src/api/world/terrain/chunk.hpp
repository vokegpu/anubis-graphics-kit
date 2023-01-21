#ifndef AGK_API_WORLD_TERRAIN_CHUNK_H
#define AGK_API_WORLD_TERRAIN_CHUNK_H

#include "api/gpu/tools.hpp"
#include "api/mesh/mesh.hpp"
#include "api/world/world_feature.hpp"
#include "api/util/math.hpp"

class chunk : public world_feature {
protected:
    bool processed {};
public:
    buffering buffer {};
    uint32_t texture {};

    explicit chunk();
    ~chunk();

    void on_create() override;
    void on_destroy() override;
    bool is_processed();
};

#endif