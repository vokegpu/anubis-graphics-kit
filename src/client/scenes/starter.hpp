#ifndef AGK_CLIENT_SCENES_START_SCENE_H
#define AGK_CLIENT_SCENES_START_SCENE_H

#include "api/feature/feature.hpp"

namespace client::scenes {
	class starter : public feature {
	public:
		void on_create();
		void on_destroy();
		void on_event(SDL_Event &sdl_event);
		void on_update() override;
		void on_render() override;
	};
}

#endif