#ifndef AGK_API_WORLD_OBJECT_H
#define AGK_API_WORLD_OBJECT_H

#include "api/feature/feature.hpp"
#include <glm/glm.hpp>
#include "api/world/material/material.hpp"

class object : public feature {
public:
    uint32_t id {};
    uint32_t model_id {};
    std::string model_tag {};

    glm::vec3 position {};
    glm::vec3 rotation {};
    glm::vec3 scale {};

    material::composition composition {};

    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_render() override;
};

#endif
