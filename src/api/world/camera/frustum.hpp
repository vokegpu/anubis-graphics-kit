#ifndef AGK_API_WORLD_FRUSTUM_H
#define AGK_API_WORLD_FRUSTUM_H

#include "api/world/world_feature.hpp"
#include <vector>
#include "api/util/math.hpp"

class frustum : public world_feature {
protected:
    glm::vec3 front {};
    glm::vec3 right {};
    glm::vec3 up {};
    glm::vec3 world_up {};
    glm::mat4 view {};
    glm::mat4 perspective {};

    glm::vec3 refer_x {};
    glm::vec3 refer_y {};
    glm::vec3 refer_z {};

    float fov {90.0f};
    float w {};
    float h {};
    float aspect {};
    float tangent {};

    float near {};
    float far {};

    util::plane top_face {};
    util::plane bottom_face {};
    util::plane right_face {};
    util::plane left_face {};
    util::plane far_face {};
    util::plane near_face {};
public:
    glm::vec3 &get_front();
    glm::vec3 &get_right();
    glm::vec3 &get_up();
    glm::vec3 &get_world_up();
    glm::mat4 &get_view();
    glm::mat4 &get_perspective();

    void set_fov(float radians_angle);
    float get_fov();
    bool viewing(const glm::mat4 &mat4x4_model, const glm::vec3 &scale, const util::aabb &axis_aligned_bounding_box);

    void process_perspective(int32_t w, int32_t h);
    void on_event(SDL_Event &sdl_event) override;
};

#endif