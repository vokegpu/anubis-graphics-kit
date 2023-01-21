#ifndef AGK_API_WORLD_MODEL_H
#define AGK_API_WORLD_MODEL_H

#include "api/world/world_feature.hpp"
#include "api/gpu/tools.hpp"

class model : public world_feature {
public:
    glm::vec4 matrix_model {};
    buffering buffer {};

    void on_create() override;
    void on_destroy() override;
};

#endif