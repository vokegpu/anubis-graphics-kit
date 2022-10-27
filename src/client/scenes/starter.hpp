#ifndef AGK_CLIENT_SCENES_START_SCENE_H
#define AGK_CLIENT_SCENES_START_SCENE_H

#include "api/feature/feature.hpp"
#include "api/gpu/buffer_builder.hpp"

namespace client::scenes {
	class starter : public feature {
	public:
		void on_create() override;
		void on_destroy() override;
		void on_event(SDL_Event &sdl_event) override;
		void on_update() override;
		void on_render() override;
	};
}

#endif