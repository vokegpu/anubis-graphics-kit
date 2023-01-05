#ifndef AGK_API_WORLD_FEATURE_H
#define AGK_API_WORLD_FEATURE_H

#include "api/feature/feature.hpp"
#include "api/world/enums/enums.hpp"
#include "api/world/pbr/material.hpp"
#include <glm/glm.hpp>

class world_feature : public feature {
protected:
    enums::state visible {enums::state::none};
    enums::priority priority {enums::priority::none};
public:
    material *p_material {nullptr};
    enums::type type {enums::type::abstract};

	glm::vec3 position {};
	glm::vec3 rotation {};
	glm::vec3 scale {1, 1, 1};

    void set_visible(enums::state enum_state, bool dispatch_event = true);
    enums::state get_visible();

    void set_priority(enums::priority enum_priority, bool dispatch_event = true);
    enums::priority get_priority();

    virtual void on_low_update();
};

#endif