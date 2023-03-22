#ifndef AGK_WORLD_TERRAIN_CHUNK_METADATA_H
#define AGK_WORLD_TERRAIN_CHUNK_METADATA_H

#include <iostream>

struct chunkmetadata {
    float frequency {};
    float amplitude {};
    float persistence {};
    float lacunarity {};
    int32_t octaves {};
};

const chunkmetadata EMPTY_CHUNK_METADATA {0.0f, 0.0f, 0.0f, 0.0f, 0};

inline bool operator==(const chunkmetadata &chunk_metadata1, const chunkmetadata &chunk_metadata2) {
    return (
            chunk_metadata1.octaves == chunk_metadata2.octaves &&
            chunk_metadata1.frequency == chunk_metadata2.frequency &&
            chunk_metadata1.amplitude == chunk_metadata2.amplitude &&
            chunk_metadata1.lacunarity == chunk_metadata2.lacunarity
    );
}

inline bool operator!=(const chunkmetadata &chunk_metadata1, const chunkmetadata &chunk_metadata2) {
    return !(chunk_metadata1 == chunk_metadata2);
}

#endif

#ifndef AGK_WORLD_TERRAIN_CHUNK_H
#define AGK_WORLD_TERRAIN_CHUNK_H

#include "gpu/gpu_buffer.hpp"
#include "world/environment/object.hpp"
#include "util/math.hpp"

class chunk : public object {
protected:
    bool processed {};
public:
    buffering buffer {};
    chunkmetadata metadata {};
    uint32_t texture {};

    explicit chunk();
    ~chunk();

    void on_create() override;
    void on_destroy() override;
    bool is_processed();
};

#endif