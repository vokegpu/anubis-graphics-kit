#include "math.hpp"

float util::clamp(float val, float min, float max) {
    return val > max ? max : (val < min ? min : val);
}

float util::min(float val, float min) {
    return val < min ? min : val;
}

float util::max(float val, float max) {
    return val > max ? max : val;
}

void util::to_grid_pos(glm::ivec2 &grid_pos, const glm::vec3 &world_pos, const glm::ivec2 &grid_size) {
    grid_pos.x = static_cast<int32_t>(world_pos.x / static_cast<float>(grid_size.x));
    grid_pos.y = static_cast<int32_t>(world_pos.z / static_cast<float>(grid_size.y));
}

void util::to_world_pos(glm::vec3 &world_pos, const glm::ivec2 &grid_pos, const glm::ivec2 &grid_size) {
    world_pos.x = static_cast<float>(grid_pos.x * grid_size.x);
    world_pos.z = static_cast<float>(grid_pos.y * grid_size.y);
}

float util::luminance(float r, float g, float b) {
    return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}

float util::lerp(float a, float b, float delta) {
    return a + (b - a) * delta;
}

bool util::check_is_on_forward_plane(const util::aabb &axis_aligned_bounding_box, const util::plane &plane) {
    const float r {
            axis_aligned_bounding_box.extents.x * glm::abs(plane.n.x) +
            axis_aligned_bounding_box.extents.y * glm::abs(plane.n.y) +
            axis_aligned_bounding_box.extents.z * glm::abs(plane.n.z)
    };

    /* Known as signed distance to plane. */
    return -r <= glm::dot(plane.n, axis_aligned_bounding_box.center) - plane.distance;
}