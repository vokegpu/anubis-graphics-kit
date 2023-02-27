#ifndef AGK_UTIL_MATH_H
#define AGK_UTIL_MATH_H

#include "asset/mesh.hpp"
#include <glm/glm.hpp>

namespace util {
    struct aabb {
        glm::vec3 min {};
        glm::vec3 max {};
    };

    void generate_aabb(util::aabb &aabb, mesh::data &mesh);
    float clamp(float val, float min, float max);
    float min(float val, float min);
    float max(float val, float max);
    float luminance(float r, float g, float b);
    float lerp(float a, float b, float delta);

    void to_grid_pos(glm::ivec2 &grid_pos, const glm::vec3 &world_pos, const glm::ivec2 &grid_size = {16, 16});
    void to_world_pos(glm::vec3 &world_pos, const glm::ivec2 &grid_pos, const glm::ivec2 &grid_size = {16, 16});

    static int const SURROUND[9] {
            -1, 0,
            1, 0,
            0, -1,
            0, 1
    };
}

#endif