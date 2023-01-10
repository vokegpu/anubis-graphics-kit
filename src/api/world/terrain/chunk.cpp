#include "chunk.hpp"
#include "api/api.hpp"

bool chunk::gen_chunk(void *p_raster_terrain, uint32_t width, uint32_t height) {
    if (this->mesh_processed) {
        return false;
    }

    auto &png_data {api::world::get()->chunk_heightmap_texture};
    auto &png_raw_data {png_data.p_data};

    glm::vec3 vertex {};
    width = png_data.w;
    height = png_data.h;
    this->scale = {0.5f, 0.5f, 0.5f};

    unsigned char r {}, g {}, b {};
    float f_r {}, f_g {}, f_b {};
    float greyscale {};

    for (uint32_t h {}; h < height; h++) {
        for (uint32_t w {}; w < width; w++) {
            r = png_raw_data[(w + h * width) * 4];
            g = png_raw_data[(w + h * width) * 4 + 1];
            b = png_raw_data[(w + h * width) * 4 + 2];

            f_r = static_cast<float>(r) / 255;
            f_g = static_cast<float>(g) / 255;
            f_b = static_cast<float>(b) / 255;

            greyscale = (f_r + f_g + f_b) / 3;
            vertex.x = (static_cast<float>(w));
            vertex.y = r;
            vertex.z = (static_cast<float>(h));

            this->meshing_data.append(mesh::type::vertex, vertex);
            this->meshing_data.append(mesh::type::color, {greyscale, greyscale, greyscale});
        }
    }

    for (uint32_t h {}; h < height - 1; h++) {
        for (uint32_t w {}; w < width - 1; w++) {
            uint32_t i1 {h * width + w};
            uint32_t i2 {h * width + w + 1};
            uint32_t i3 {(h + 1) * width + w};
            uint32_t i4 {(h + 1) * width + w + 1};

            this->meshing_data.append(mesh::type::vertex, i1);
            this->meshing_data.append(mesh::type::vertex, i3);
            this->meshing_data.append(mesh::type::vertex, i2);

            this->meshing_data.append(mesh::type::vertex, i2);
            this->meshing_data.append(mesh::type::vertex, i3);
            this->meshing_data.append(mesh::type::vertex, i4);
            this->meshing_data.faces += 6;
        }
    }

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

chunk::chunk() {

}

chunk::~chunk() {

}

void chunk::on_create() {
    feature::on_create();
    this->visible = enums::state::enable;
}

void chunk::on_destroy() {
    feature::on_destroy();
    this->buffering.free_buffers();
}

void chunk::set_mesh_processed() {
    this->mesh_processed = true;
}
