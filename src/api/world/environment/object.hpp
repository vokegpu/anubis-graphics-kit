#ifndef AGK_API_WORLD_MODEL_OBJECT_H
#define AGK_API_WORLD_MODEL_OBJECT_H

#include "api/world/world_feature.hpp"
#include "api/world/model/model.hpp"

class object : public world_feature {
public:
    model *p_model {};
};

#endif