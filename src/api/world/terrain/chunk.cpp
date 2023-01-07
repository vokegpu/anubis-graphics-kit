#include "chunk.hpp"

bool chunk::gen_chunk(void *p_raster_terrain, int32_t min, int32_t max) {
    if (this->processed) {
        return;
    }

    auto &v {this->meshing_data.get_float_list(mesh::type::vertex)};
    v.clear();

    this->meshing_data.faces = 0;
    glm::vec3 v1 {};
    glm::vec3 v2 {};
    glm::vec3 v3 {};

    for (int32_t x {}; x < min; x++) {
        for (int32_t y {}; y < max; y++) {
            v1.x = static_cast<float>(x);
            v1.y = static_cast<float>(0);
            v1.z = static_cast<float>(y);

            this->meshing_data.append(mesh::type::vertex, v1);
            this->meshing_data.faces++;
        }
    }

    this->buffering.invoke();
    this->buffering.bind({GL_ARRAY_BUFFER, GL_FLOAT});
    this->buffering.send(sizeof(float) * v.size(), v.data(), GL_STATIC_DRAW);
    this->buffering.attach(3);
    this->buffering.revoke();

    this->processed = true;
}

bool chunk::is_processed() {
    return this->processed;
}