#ifndef AGK_API_WORLD_MODEL_H
#define AGK_API_WORLD_MODEL_H

#include "api/world/world_feature.hpp"
#include "api/gpu/tools.hpp"
#include "api/util/math.hpp"

class model : public world_feature {
public:
    buffering buffer {};
    std::string tag {};
    util::aabb axis_aligned_bounding_box {};

    void on_create() override;
    void on_destroy() override;
};

#endif