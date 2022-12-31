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

    std::vector<uint32_t> framebuffer_list {};
public:
    glm::vec3 &get_front();
    glm::vec3 &get_right();
    glm::vec3 &get_up();
    glm::vec3 &get_world_up();
    glm::mat4 &get_view();

    void add_framebuffer(uint32_t framebuffer);
    std::vector<uint32_t> &get_framebuffer_list();
};

#endif