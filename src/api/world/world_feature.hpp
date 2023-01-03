#ifndef AGK_API_WORLD_FEATURE_H
#define AGK_API_WORLD_FEATURE_H

#include "api/feature/feature.hpp"
#include "api/world/enums/enums.hpp"
#include "api/world/pbr/material.hpp"
#include <glm/glm.hpp>

class world_feature : public feature {
public:
	enums::state visible {enums::state::disable};
    pbrm *p_pbrm {nullptr};
    enums::type type {enums::type::abstract};

	glm::vec3 position {};
	glm::vec3 rotation {};
	glm::vec3 scale {};
};

#endif