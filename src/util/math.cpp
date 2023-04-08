#include "math.hpp"
#include <cfloat>

glm::vec3 util::generatetangent(glm::vec3 *pv, glm::vec2 *pt) {
    if (pv == nullptr || pt == nullptr) {
        return {1.0f, 0.0f, 0.0f};
    }

    glm::vec3 &v1 {pv[0]};
    glm::vec3 &v2 {pv[1]};
    glm::vec3 &v3 {pv[2]};

    glm::vec2 &t1 {pt[0]};
    glm::vec2 &t2 {pt[1]};
    glm::vec2 &t3 {pt[2]};

    glm::vec3 vedge1 {v2 - v1};
    glm::vec3 vedge2 {v3 - v1};

    glm::vec2 tedge1 {t2 - t1};
    glm::vec2 tedge2 {t3 - t1};

    glm::vec3 tangent {};
    float invertedgesq {tedge2.y * tedge1.x - tedge2.x * tedge1.y};
    if (fabsf(invertedgesq) < FLT_EPSILON) {
        tangent.x = 1.0f;
    } else {
        tangent.x = invertedgesq * (tedge2.y * vedge1.x - tedge1.y * vedge2.x);
        tangent.y = invertedgesq * (tedge2.y * vedge1.y - tedge1.y * vedge2.y);
        tangent.z = invertedgesq * (tedge2.y * vedge1.z - tedge1.y * vedge2.z);
    }

    return tangent;
}

float util::clamp(float val, float min, float max) {
    return val > max ? max : (val < min ? min : val);
}

float util::min(float val, float min) {
    return val < min ? min : val;
}

float util::max(float val, float max) {
    return val > max ? max : val;
}

void util::togridpos(glm::ivec2 &grid_pos, const glm::vec3 &world_pos, const glm::ivec2 &grid_size) {
    grid_pos.x = static_cast<int32_t>(world_pos.x / static_cast<float>(grid_size.x));
    grid_pos.y = static_cast<int32_t>(world_pos.z / static_cast<float>(grid_size.y));
}

void util::toworldpos(glm::vec3 &world_pos, const glm::ivec2 &grid_pos, const glm::ivec2 &grid_size) {
    world_pos.x = static_cast<float>(grid_pos.x * grid_size.x);
    world_pos.z = static_cast<float>(grid_pos.y * grid_size.y);
}

float util::luminance(float r, float g, float b) {
    return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}

float util::lerp(float a, float b, float delta) {
    return a + (b - a) * delta;
}

void util::generateaabb(util::aabb &aabb, stream::mesh &mesh) {
    auto &list {mesh.get_float_list(stream::type::vertex)};
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
