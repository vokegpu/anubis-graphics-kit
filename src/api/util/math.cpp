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

void util::transform_to_grid_pos(glm::ivec2 &grid_pos, const glm::vec3 &world_pos, const glm::ivec2 &grid_size) {
    grid_pos.x = static_cast<int32_t>(world_pos.x / static_cast<float>(grid_size.x));
    grid_pos.y = static_cast<int32_t>(world_pos.z / static_cast<float>(grid_size.y));
}

void util::transform_to_world_pos(glm::vec3 &world_pos, const glm::ivec2 &grid_pos, const glm::ivec2 &grid_size) {
    world_pos.x = static_cast<float>(grid_pos.x * grid_size.x);
    world_pos.z = static_cast<float>(grid_pos.y * grid_size.y);
}
