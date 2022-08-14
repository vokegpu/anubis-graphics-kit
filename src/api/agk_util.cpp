#include "agk_util.hpp"

#include <SDL2/SDL.h>
#include <fstream>

uint64_t agk_clock::elapsed_ticks = 0;
uint64_t agk_clock::ticks_going_on = 0;
uint64_t agk_clock::current_ticks = 0;
uint64_t agk_clock::fps_interval = 0;
uint8_t agk_clock::fps_limit = 0;
uint32_t agk_clock::fps = 0;
uint32_t agk_clock::fps_elapsed = 0;
uint64_t agk_clock::elapsed_milliseconds_fps_cycle = 0;

std::map<std::string, bool> agk_util::keymap;
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

void agk_util::log(const std::string &name) {
    const std::string property = "[MAIN] " + name;
    std::cout << property.c_str() << "\n";
}

bool agk_util::open_file(agk_source &source, const std::string &path) {
    if (!agk_util::file_exists(path)) {
        agk_util::log("Could not open '" + path + "'.");
        return false;
    }

    bool flag = false;

    // TODO modes for get the data from opened file.
    if (true) {
        std::ifstream ifs(path.c_str());
        std::string string_builder;

        if (ifs.is_open()) {
            std::string string_buffer;

            while (getline(ifs, string_buffer)) {
                string_builder += "\n" + string_buffer;
            }

            ifs.close();
            source.data1 = string_builder;
        }

        flag = true;
    }

    return flag;
}

bool agk_util::file_exists(const std::string &path) {
    FILE* f = fopen(path.c_str(), "r");
    bool flag = f != NULL;

    if (flag) {
        fclose(f);
    }

    return flag;
}

void agk_util::keyboard(SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_KEYDOWN: {
            agk_util::keymap[SDL_GetKeyName(sdl_event.key.keysym.sym)] = true;
            break;
        }

        case SDL_KEYUP: {
            agk_util::keymap[SDL_GetKeyName(sdl_event.key.keysym.sym)] = false;
            break;
        }
    }
}

void agk_util::init() {

}
