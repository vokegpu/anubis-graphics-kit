
#include <glm/glm.hpp>
#include "api/feature/feature.hpp"

#ifndef AGK_API_WORLD_CAMERA_H
#define AGK_API_WORLD_CAMERA_H

class camera : public feature {
protected:
	glm::mat4 view {};
public:
	float mouse_sensitivity {};
	float field_of_view {};
	bool lock {};

	glm::vec3 rotation {};
	glm::vec3 position {};
	glm::vec3 front {};
	glm::vec3 right {};
	glm::vec3 up {};
	glm::vec3 world_up {};

	glm::mat4 &get_matrix();

	void on_create() override;
	void on_destroy() override;
	void on_event(SDL_Event &sdl_event) override;
	void on_update() override;
};

#endif