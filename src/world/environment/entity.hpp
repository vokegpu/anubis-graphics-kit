#ifndef AGK_WORLD_ENVIRONMENT_ENTITY_H
#define AGK_WORLD_ENVIRONMENT_ENTITY_H

#include "object.hpp"

class entity : public object {
public:
    float speed_base {0.2867f};
    glm::vec3 velocity {};
    glm::vec3 acceleration {};

    explicit entity();
    ~entity();

    void on_update() override;
};

#endif