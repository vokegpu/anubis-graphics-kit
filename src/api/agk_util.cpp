#include "agk_util.hpp"

#include <SDL2/SDL.h>

float agk_clock::delta_time = 0.0f;
uint8_t agk_clock::fps = 0;

void agk_clock::set_fps(uint8_t new_fps) {
    if (new_fps != fps_limit) {
        fps_interval = new_fps;
        fps_interval = (1000 / new_fps);
    }
}

void agk_clock::run() {
    current_ticks = SDL_GetTicks64();
    ticks_going_on = current_ticks - elapsed_ticks;
}

bool agk_clock::ticks_running_asynchronous() {
    return ticks_going_on > fps_interval;
}

void agk_clock::sync_running_ticks() {
    elapsed_ticks = current_ticks;
    delta_time = static_cast<float>(current_ticks) / 100.0f;

    if ()
}

void util::log(const std::string &name) {
    const std::string property = "[MAIN] " + name;
    std::cout << property.c_str() << "\n";
}
