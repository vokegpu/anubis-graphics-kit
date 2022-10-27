#include "input.hpp"
#include <algorithm>

void input::on_event(SDL_Event &sdl_event) {
	switch (sdl_event.type) {
		case SDL_KEYDOWN: {
			for (std::string key_string {SDL_GetKeyName(sdl_event.key.keysym.sym)}; !key_string.empty(); key_string) {
				std::transform(key_string.begin(), key_string.end(), key_string.begin(), ::tolower);
				this->input_map[key_string] = true;
				break;
			}
			break;
		}

		case SDL_KEYUP: {
			for (std::string key_string {SDL_GetKeyName(sdl_event.key.keysym.sym)}; !key_string.empty(); key_string) {
				std::transform(key_string.begin(), key_string.end(), key_string.begin(), ::tolower);
				this->input_map[key_string] = false;
				break;
			}
			break;
		}

        case SDL_MOUSEBUTTONDOWN: {
            switch (sdl_event.button.button) {
                case SDL_BUTTON_LEFT: {
                    this->input_map["mouse-left"] = true;
                    break;
                }

                case SDL_BUTTON_MIDDLE: {
                    this->input_map["mouse-middle"] = true;
                    break;
                }

                case SDL_BUTTON_RIGHT: {
                    this->input_map["mouse-right"] = true;
                    break;
                }
            }
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            switch (sdl_event.button.button) {
                case SDL_BUTTON_LEFT: {
                    this->input_map["mouse-left"] = false;
                    break;
                }

                case SDL_BUTTON_MIDDLE: {
                    this->input_map["mouse-middle"] = false;
                    break;
                }

                case SDL_BUTTON_RIGHT: {
                    this->input_map["mouse-right"] = false;
                    break;
                }
            }
            break;
        }
	}
}

void input::on_update() {

}