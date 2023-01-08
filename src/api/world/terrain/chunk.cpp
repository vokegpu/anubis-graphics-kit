#include "chunk.hpp"
#include "api/api.hpp"

bool chunk::gen_chunk(void *p_raster_terrain, int32_t width, int32_t height) {
    if (this->mesh_processed) {
        return false;
    }

    auto &png_data {api::world::get()->png_chunk_data};
    glm::vec3 v1 {}, v2 {}, v3 {}, v4 {};
    float metadata[376][376][3] {};

    width = 376;
    height = 376;

    int32_t x {}, z {};
    float scale {2.0f};
    float highest {4.0f};

    float yscale {64.0f / 256.0f};
    float yshift {16.0f};

    for (x = 0; x < width; x++) {
        for (z = 0; z < height; z++) {
            unsigned char *p_texel {png_data.data() + (x + width * z) * GL_RGB};
            unsigned char y {p_texel[0]};

            metadata[x][z][0] = static_cast<float>(x);
            metadata[x][z][1] = static_cast<float>(y) * yscale - yshift;
            metadata[x][z][2] = static_cast<float>(z);
        }
    }

    for (z = 0; z < height - 1; z++) {
        for (x = 0; x < width - 1; x++) {
            v1.x = metadata[x][z][0];
            v1.y = metadata[x][z][1] * highest;
            v1.z = metadata[x][z][2];
            this->meshing_data.append(mesh::type::vertex, v1);

            v2.x = metadata[x + 1][z][0];
            v2.y = metadata[x + 1][z][1] * highest;
            v2.z = metadata[x + 1][z][2];
            this->meshing_data.append(mesh::type::vertex, v2);

            v3.x = metadata[x][z + 1][0];
            v3.y = metadata[x][z + 1][1] * highest;
            v3.z = metadata[x][z + 1][2];
            this->meshing_data.append(mesh::type::vertex, v3);

            v4.x = metadata[x + 1][z + 1][0];
            v4.y = metadata[x + 1][z + 1][1] * highest;
            v4.z = metadata[x + 1][z + 1][2];
            this->meshing_data.append(mesh::type::vertex, v4);

            /* Three times because this is a triangle. */
            this->meshing_data.append(mesh::type::color, {metadata[x][z][1],         metadata[x][z][1],         metadata[x][z][1]});
            this->meshing_data.append(mesh::type::color, {metadata[x + 1][z][1],     metadata[x + 1][z][1],     metadata[x + 1][z][1]});
            this->meshing_data.append(mesh::type::color, {metadata[x][z + 1][1],     metadata[x][z + 1][1],     metadata[x][z + 1][1]});
            this->meshing_data.append(mesh::type::color, {metadata[x + 1][z + 1][1], metadata[x + 1][z + 1][1], metadata[x + 1][z + 1][1]});
            this->meshing_data.faces += 3;
        }
    }

    util::log("Chunk generated " + std::to_string(this->meshing_data.faces) + " faces");
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
