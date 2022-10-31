#ifndef AGK_API_WORLD_OBJECT_H
#define AGK_API_WORLD_OBJECT_H

#include "api/feature/feature.hpp"
#include <glm/glm.hpp>
#include <string>
#include "api/world/material/material.hpp"

class object : public feature {
public:
    int32_t id {};
    int32_t model_id {-1};
    std::string model_tag {};

    glm::vec3 position {};
    glm::vec3 rotation {};
    glm::vec3 scale {1, 1, 1};

    material::data* material {};
    object(material::data*);
    ~object();

    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event&) override;
    void on_update() override;
    void on_render() override;
};

#endif
