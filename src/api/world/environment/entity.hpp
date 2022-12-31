#ifndef AGK_WORLD_API_MODEL_ENTITY_H
#define AGK_WORLD_API_MODEL_ENTITY_H

#include "api/world/world_feature.hpp"
#include "api/world/model/model.hpp"

class entity : public world_feature {
public:
    model *p_model {};
};

#endif