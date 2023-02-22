#ifndef AGK_API_UTIL_MATH_H
#define AGK_API_UTIL_MATH_H

#include "asset/mesh.hpp"
#include <glm/glm.hpp>

namespace util {
    typedef struct aabb {
        glm::vec3 center {};
        glm::vec3 extents {};

        aabb() = default;
        aabb(const glm::vec3 &min, const glm::vec3 &max) : center((min + max) * 0.5f), extents(max.x - center.x, max.y - center.y, max.z - center.z) {}
    } aabb;

    typedef struct plane {
        glm::vec3 n {};
        float distance {};

        plane() = default;
        plane(const glm::vec3 &p, const glm::vec3 &normal) : n(glm::normalize(n)), distance(glm::dot(normal, p)) {}
    } plane;
    
    float clamp(float val, float min, float max);
    float min(float val, float min);
    float max(float val, float max);
    float luminance(float r, float g, float b);
    float lerp(float a, float b, float delta);

    void to_grid_pos(glm::ivec2 &grid_pos, const glm::vec3 &world_pos, const glm::ivec2 &grid_size = {16, 16});
    void to_world_pos(glm::vec3 &world_pos, const glm::ivec2 &grid_pos, const glm::ivec2 &grid_size = {16, 16});
    bool check_is_on_forward_plane(const util::aabb &axis_aligned_bounding_box, const util::plane &plane);

    static int const SURROUND[9] {
        -1, 0,
        1, 0,
        0, -1,
        0, 1
    };
}

#endif