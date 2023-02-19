#include "input.hpp"
#include "api/util/env.hpp"
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
            std::string tag {"mouse-"};
            tag += std::to_string(sdl_event.button.button);
            this->input_map[tag] = true;
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            std::string tag {"mouse-"};
            tag += std::to_string(sdl_event.button.button);
            this->input_map[tag] = false;
            break;
        }
    }
}

void input::on_update() {

}

input::~input() {

}
