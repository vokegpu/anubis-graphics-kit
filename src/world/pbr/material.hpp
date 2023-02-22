#ifndef AGK_WORLD_PBR_MATERIAL_H
#define AGK_WORLD_PBR_MATERIAL_H

#include <glm/glm.hpp>
#include "world/enums/enums.hpp"

class material {
protected:
	int32_t texture {};
	glm::vec3 color {};
    float rough {0.43f};
	enums::material type {enums::material::empty};
public:
	explicit material(enums::material material_type);
	~material();

	void set_color(glm::vec3 color3f);
	glm::vec3 get_color();

	void set_texture(int32_t id);
	int32_t get_texture();

    void set_rough_based_type(enums::material material_type);
    void set_rough(float rough_value);
    float get_rough();

	enums::material get_type();
};

#endif