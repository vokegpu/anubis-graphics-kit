#ifndef AGK_API_CORE_H
#define AGK_API_CORE_H

#include <SDL2/SDL.h>
#include "api/gc/gc.hpp"
#include "api/renderer/shading_access.hpp"

class core {
public:
	SDL_Window* root {nullptr};

	int32_t screen_width {1600};
	int32_t screen_height {900};

	bool mainloop {};
	uint64_t fps {60};
	uint64_t display_fps {};

	feature* current_scene {nullptr};
	gc garbage_collector {};
    shading_manager shader_manger {};

};

#endif