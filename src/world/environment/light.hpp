#ifndef AGK_WORLD_ENVIRONMENT_LIGHT_H
#define AGK_WORLD_ENVIRONMENT_LIGHT_H

#include "object.hpp"

class light : public object {
protected:
    bool low_update_ticking {};
public:
    int32_t index {};
    glm::vec3 intensity {1.0f, 1.0f, 1.0f};
    bool directional {};
public:
    explicit light();
    ~light();

    void update();
    void on_low_update() override;
};

#endif