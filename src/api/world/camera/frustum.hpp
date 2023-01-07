#ifndef AGK_API_WORLD_FRUSTUM_H
#define AGK_API_WORLD_FRUSTUM_H

#include <vector>
#include "api/world/world_feature.hpp"

class frustum : public world_feature {
protected:
    glm::vec3 front {};
    glm::vec3 right {};
    glm::vec3 up {};
    glm::vec3 world_up {};
    glm::mat4 view {};
    glm::mat4 perspective {};

    std::vector<uint32_t> framebuffer_list {};
    float fov {90.0f};
public:
    glm::vec3 &get_front();
    glm::vec3 &get_right();
    glm::vec3 &get_up();
    glm::vec3 &get_world_up();
    glm::mat4 &get_view();
    glm::mat4 &get_perspective();

    void set_fov(float radians_angle);
    float get_fov();

    void add_framebuffer(uint32_t framebuffer);
    std::vector<uint32_t> &get_framebuffer_list();

    void process_perspective(int32_t w, int32_t h);
    void on_event(SDL_Event &sdl_event) override;
};

#endif