#ifndef AGK_API_CORE_H
#define AGK_API_CORE_H

#include <SDL2/SDL.h>
#include "api/gc/gc.hpp"

class core {
public:
	SDL_Window* root {};

	int32_t screen_width {1600};
	int32_t screen_height {900};

	bool mainloop {true};
	uint64_t fps {60};
	uint64_t display_fps {};

	feature* current_scene {nullptr};
	gc garbage_collector {};

};

#endif