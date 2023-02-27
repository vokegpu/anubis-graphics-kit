#ifndef AGK_WORLD_FRUSTUM_H
#define AGK_WORLD_FRUSTUM_H

#include "world/environment/object.hpp"
#include <vector>
#include "util/math.hpp"

struct viewplane {
    glm::vec3 n {};
    float distance {};

    inline viewplane() = default;
    void set_plane(const glm::vec3 &p, float dist);
};

class frustum : public object {
protected:
    glm::vec3 front {};
    glm::vec3 right {};
    glm::vec3 up {};
    glm::vec3 world_up {};
    glm::mat4 view {};
    glm::mat4 perspective {};
    glm::mat4 mvp {};

    glm::vec3 rx {};
    glm::vec3 ry {};
    glm::vec3 rz {};

    float fov {90.0f};
    float w {};
    float h {};
    float aspect {};
    float tangent {};

    float near {};
    float far {};

    viewplane planes[6] {};
public:
    frustum() = default;
    ~frustum() = default;

    glm::vec3 &get_front();
    glm::vec3 &get_right();
    glm::vec3 &get_up();
    glm::vec3 &get_world_up();
    glm::mat4 &get_view();
    glm::mat4 &get_perspective();
    glm::mat4 &get_mvp();

    void set_fov(float radians_angle);
    float get_fov();
    bool viewing(glm::mat4 &mat4x4_model, util::aabb &aabb);

    void process_perspective(int32_t w, int32_t h);
    void on_event(SDL_Event &sdl_event) override;
};

#endif