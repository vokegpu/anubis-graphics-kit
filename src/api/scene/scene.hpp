#ifndef AGK_SCENE_H
#define AGK_SCENE_H

#include "api/core/feature.hpp"

class scene : public feature {
public:
	void on_create() override;
	void on_destroy() override;
	void on_event(SDL_Event &sdl_event) override;
	void on_update() override;
	void on_render() override;
};

#endif