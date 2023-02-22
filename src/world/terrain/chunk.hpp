#ifndef AGK_API_WORLD_TERRAIN_CHUNK_DATA_H
#define AGK_API_WORLD_TERRAIN_CHUNK_DATA_H

#include <iostream>

struct chunk_metadata {
    float frequency {};
    float amplitude {};
    float persistence {};
    float lacunarity {};
    int32_t octaves {};
};

const chunk_metadata EMPTY_CHUNK_METADATA {0.0f, 0.0f, 0.0f, 0.0f, 0};

inline bool operator==(const chunk_metadata &chunk_metadata1, const chunk_metadata &chunk_metadata2) {
    return (
        chunk_metadata1.octaves == chunk_metadata2.octaves &&
        chunk_metadata1.frequency == chunk_metadata2.frequency &&
        chunk_metadata1.amplitude == chunk_metadata2.amplitude &&
        chunk_metadata1.lacunarity == chunk_metadata2.lacunarity
    );
}

inline bool operator!=(const chunk_metadata &chunk_metadata1, const chunk_metadata &chunk_metadata2) {
    return !(chunk_metadata1 == chunk_metadata2);
}

#endif

#ifndef AGK_API_WORLD_TERRAIN_CHUNK_H
#define AGK_API_WORLD_TERRAIN_CHUNK_H

#include "gpu/tools.hpp"
#include "asset/mesh.hpp"
#include "world/environment/object.hpp"
#include "util/math.hpp"

class chunk : public object {
protected:
    bool processed {};
public:
    buffering buffer {};
    chunk_metadata metadata {};
    uint32_t texture {};
    std::vector<int32_t> vegetation {};

    explicit chunk();
    ~chunk();

    void on_create() override;
    void on_destroy() override;
    bool is_processed();
};

#endif