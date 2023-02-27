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

void util::generate_aabb(util::aabb &aabb, mesh::data &mesh) {
    auto &list {mesh.get_float_list(mesh::type::vertex)};
    glm::vec3 min {glm::vec3(std::numeric_limits<float>::max())};
    glm::vec3 max {glm::vec3(std::numeric_limits<float>::min())};

    for (int32_t it {}; it < list.size() / 3; it++) {
        const float *p_vec {&list.at(it * 3)};
        min.x = std::min(min.x, p_vec[0]);
        min.y = std::min(min.y, p_vec[1]);
        min.z = std::min(min.z, p_vec[2]);

        max.x = std::max(max.x, p_vec[0]);
        max.y = std::max(max.y, p_vec[1]);
        max.z = std::max(max.z, p_vec[2]);
    }

    aabb = {min, max};
}
