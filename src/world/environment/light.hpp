#ifndef AGK_API_WORLD_ENVIRONMENT_LIGHT_H
#define AGK_API_WORLD_ENVIRONMENT_LIGHT_H

#include "object.hpp"
#include "asset/model.hpp"

class light : public object {
protected:
    bool low_update_ticking {};
public:
    ::asset::model *p_model {};
    int32_t index {};

    glm::vec3 intensity {1.0f, 1.0f, 1.0f};
    bool directional {};

    explicit light(::asset::model *p_model_linked = nullptr);
    ~light();

    void update();
    void on_low_update() override;
};

#endif