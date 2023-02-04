#ifndef AGK_API_WORLD_ENVIRONMENT_LIGHT_H
#define AGK_API_WORLD_ENVIRONMENT_LIGHT_H

#include "api/world/world_feature.hpp"
#include "api/world/model/model.hpp"

class light : public world_feature {
protected:
    bool low_update_ticking {};
public:
    model *p_model {};
    int32_t index_light {};

    glm::vec3 intensity {1.0f, 1.0f, 1.0f};
    bool directional {};

    explicit light(model *p_model_linked = nullptr);
    ~light();

    void update();
    void on_low_update() override;
};

#endif