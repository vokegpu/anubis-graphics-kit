#ifndef AGK_WORLD_ENVIRONMENT_ENTITY_H
#define AGK_WORLD_ENVIRONMENT_ENTITY_H

#include "object.hpp"
#include "asset/model.hpp"

class entity : public object {
public:
    ::asset::model *p_model {};

    float speed_base {0.2867f};
    glm::vec3 velocity {};
    glm::vec3 acceleration {};

    explicit entity(::asset::model *p_model_linked = nullptr);
    ~entity();

    void on_update() override;
};

#endif