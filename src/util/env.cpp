#include "env.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <sstream>
#include <string>
#include "GL/glew.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

bool util::log(const std::string &log_message) {
    const std::string full_log_message {"[AGK] " + log_message};
    std::cout << full_log_message.c_str()<< '\n';
    return true;
}

bool util::reset(util::timing &timing) {
    timing.ticks = SDL_GetTicks64();
    timing.ms_elapsed = timing.ticks;
    return true;
}

bool util::reach(util::timing &timing, uint64_t ms) {
    timing.ticks = SDL_GetTicks64();

    if ((timing.ms_current = timing.ticks - timing.ms_elapsed) > ms) {
        return true;
    }

    return false;
}

bool util::reset_when(util::timing &timing, uint64_t ms) {
    timing.ticks = SDL_GetTicks64();

    if ((timing.ms_current = timing.ticks - timing.ms_elapsed) > ms) {
        timing.ms_elapsed = timing.ticks;
        return true;
    }

    return false;
}

void util::split(std::vector<std::string> &list, std::string_view str, const char &divisor) {
    std::string string_buffer {};
    std::stringstream ss(str.data());
    list.clear();

    while (std::getline(ss, string_buffer, divisor)) {
        list.push_back(string_buffer);
    }
}

void util::log2f(float x, float y) {
    util::log("XY ["+ std::to_string(x) + ", " + std::to_string(y) + "]");
}
