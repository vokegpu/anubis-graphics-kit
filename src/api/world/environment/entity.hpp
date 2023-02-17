#ifndef AGK_WORLD_API_MODEL_ENTITY_H
#define AGK_WORLD_API_MODEL_ENTITY_H

#include "api/world/world_feature.hpp"
#include "api/world/model/model.hpp"

class entity : public world_feature {
public:
    model *p_model {};

    float speed_base {0.2867f};
    glm::vec3 velocity {};
    glm::vec3 acceleration {};

    explicit entity(model *p_model_linked = nullptr);
    ~entity();

    void on_update() override;
};

#endif