#include <glm/glm.hpp>
#include "api/world/enums/enums.hpp"

#ifndef AGK_WORLD_PBR_MATERIAL_H
#define AGK_WORLD_PBR_MATERIAL_H

class pbrm {
protected:
	int32_t texture {};
	glm::vec3 color {};
	enums::material type {enums::material::empty};
public:
	pbrm();
	~pbrm();

	void set_color(glm::vec3 color3f);
	glm::vec3 get_color();

	void set_texture(int32_t id);
	int32_t get_texture();

	enums::material get_type();
};

#endif

#ifndef AGK_WORLD_PBR_MATERIAL_LIGHT_H
#define AGK_WORLD_PBR_MATERIAL_LIGHT_H

class pbrm_light : public pbrm {
protected:
	glm::vec3 intensity {0.4f, 0.4f, 0.4f};
public:
	pbrm_light();
	~pbrm_light();

	void set_intensity(glm::vec3 light_intensity);
	glm::vec3 get_intensity();
};

#endif

#ifndef AGK_WORLD_PBR_MATERIAL_SOLID_H
#define AGK_WORLD_PBR_MATERIAL_SOLID_H

class pbrm_solid : public pbrm {
protected:
	float rough {0.43f};
public:
	pbrm_solid(enums::material mtype);
	~pbrm_solid();

	void set_rough_based_type(enums::material mtype);
	void set_rough(float rough_value);
	float get_rough();
};

#endif