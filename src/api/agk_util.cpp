#include "agk_util.hpp"

#include <SDL2/SDL.h>

uint64_t agk_clock::elapsed_ticks = 0;
uint64_t agk_clock::ticks_going_on = 0;
uint64_t agk_clock::current_ticks = 0;
uint64_t agk_clock::fps_interval = 0;
uint8_t agk_clock::fps_limit = 0;
uint32_t agk_clock::fps = 0;
uint32_t agk_clock::fps_elapsed = 0;
uint64_t agk_clock::elapsed_milliseconds_fps_cycle = 0;

float agk_clock::delta_time = 0.0f;

void agk_clock::set_fps(uint8_t new_fps) {
    if (new_fps != fps_limit) {
        fps_interval = new_fps;
        fps_interval = (1000 / new_fps);
        elapsed_ticks = SDL_GetTicks64();
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
    elapsed_milliseconds_fps_cycle += ticks_going_on;
    delta_time = static_cast<float>(ticks_going_on) / 100.0f;
}

void agk_clock::check_frame_rate() {
    fps_elapsed++;

    if (elapsed_milliseconds_fps_cycle > 1000) {
        fps = (uint32_t) fps_elapsed;
        elapsed_milliseconds_fps_cycle = 0;
    }
}

void util::log(const std::string &name) {
    const std::string property = "[MAIN] " + name;
    std::cout << property.c_str() << "\n";
}
