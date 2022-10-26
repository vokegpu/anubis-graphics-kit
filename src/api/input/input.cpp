#include "input.hpp"
#include <algorithm>

void input::on_event(SDL_Event &sdl_event) {
	switch (sdl_event.type) {
		case SDL_KEYDOWN: {
			for (std::string key_string {SDL_GetKeyName(sdl_event.key.keysym.sym)}; !key_string.empty(); key_string) {
				std::transform(key_string.begin(), key_string.end(), key_string.begin(), ::tolower);
				this->input_map[key_string] = true;
			}
			break;
		}

		case SDL_KEYUP: {
			for (std::string key_string {SDL_GetKeyName(sdl_event.key.keysym.sym)}; !key_string.empty(); key_string) {
				std::transform(key_string.begin(), key_string.end(), key_string.begin(), ::tolower);
				this->input_map[key_string] = false;
			}
			break;
		}
	}
}

void input::on_update() {

}