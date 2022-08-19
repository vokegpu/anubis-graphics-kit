#pragma once
#ifndef AGK_UTIL_H
#define AGK_UTIL_H

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <map>

/**
 * The AGK clock for manage fps and stuff.
 */
struct agk_clock {
    static uint64_t elapsed_ticks;
    static uint64_t ticks_going_on;
    static uint64_t current_ticks;

    static uint64_t fps_interval;
    static uint8_t fps_limit;

    static uint32_t fps;
    static uint32_t fps_elapsed;
    static uint64_t elapsed_milliseconds_fps_cycle;

    static float delta_time;

    static void set_fps(uint8_t new_fps);
    static void run();
    static bool ticks_running_asynchronous();
    static void sync_running_ticks();
    static void check_frame_rate();
};

/**
 * File source.
 **/
struct agk_source {
    std::string data1;
};

/**
 * The utilities of program.
 **/
namespace util {
    extern std::map<std::string, bool> keymap;

    void init();
    void log(const std::string &name);
    void keyboard(SDL_Event &sdl_event);

    bool open_file(agk_source &source, const std::string &path);
    bool file_exists(const std::string &path);
};

#endif