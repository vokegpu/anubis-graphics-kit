#ifndef AGK_API_UTIL_MATH_H
#define AGK_API_UTIL_MATH_H

#include "api/mesh/mesh.hpp"
#include <glm/glm.hpp>

namespace util {
    float clamp(float val, float min, float max);
    float min(float val, float min);
    float max(float val, float max);
    float luminance(float r, float g, float b);

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