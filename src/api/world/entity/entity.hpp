#ifndef AGK_API_WORLD_ENTITY_H
#define AGK_API_WORLD_ENTITY_H

#include "api/feature/feature.hpp"
#include <glm/glm.hpp>

class entity : public feature {
public:
	glm::vec3 position {};
	glm::vec3 velocity {};
	glm::vec3 scale {};
	float yaw {}, pitch {};

	void on_create() override;
	void on_destroy() override;
	void on_event(SDL_Event &sdl_event) override;
	void on_update() override;
	void on_render() override;
};

#endif