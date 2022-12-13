#ifndef AGK_API_WORLD_FEATURE_H
#define AGK_API_WORLD_FEATURE_H

#include "api/feature/feature.hpp"
#include "api/world/enums/enums.hpp"

class world_feature : public feature {
public:
	int32_t id {};
	enums::state visible {enums::state::disable};

	glm::vec3 position {};
	glm::vec3 rotation {};
	glm::vec3 scale {};
};

#endif