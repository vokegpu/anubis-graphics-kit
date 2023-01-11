#include "env.hpp"
#include <SDL2/SDL.h>
#include <sstream>
#include <string>
#include <lodepng/lodepng.h>
#include "GL/glew.h"

void util::log(const std::string &log_message) {
    const std::string full_log_message {"[AGK] " + log_message};
    std::cout << full_log_message.c_str()<< '\n';
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

bool util::resetifreach(util::timing &timing, uint64_t ms) {
    timing.ticks = SDL_GetTicks64();

    if ((timing.ms_current = timing.ticks - timing.ms_elapsed) > ms) {
        timing.ms_elapsed = timing.ticks;
        return true;
    }

    return false;
}

void util::split(std::vector<std::string> &list, std::string_view str, const char divisor) {
    std::string string_buffer {};
    std::stringstream ss(str.data());
    list.clear();

    while (std::getline(ss, string_buffer, divisor)) {
        list.push_back(string_buffer);
    }
}

bool util::loadtexture(util::texture *p_texture) {
    unsigned error = lodepng_decode32_file(&p_texture->p_data, &p_texture->w, &p_texture->h, p_texture->path.data());
    if (error) {
        const std::string msg {"Failed to read read texture '" + p_texture->path + "'"};
        util::log(msg);
        return true;
    }

    return false;
}

uint32_t util::createtexture(util::texture *p_texture) {
    uint32_t tex_id {};

    /* Gen a GL object (texture). */
    glGenTextures(GL_TEXTURE_2D, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    /* Pass format of png, dimension of image, unsigned short type & data from texture. */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int32_t) p_texture->w, (int32_t) p_texture->h, 0, GL_RGBA, GL_UNSIGNED_SHORT, p_texture->p_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Return the GL object. */
    return tex_id;
}
