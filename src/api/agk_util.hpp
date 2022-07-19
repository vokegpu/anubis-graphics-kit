#pragma once
#ifndef AGK_UTIL_H
#define AGK_UTIL_H

#include <iostream>
#include <string>

/**
 * The AGK clock for manage fps and stuff.
 */
struct agk_clock {
    static uint64_t elapsed_ticks;
    static uint64_t ticks_going_on;
    static uint64_t current_ticks;

    static uint64_t fps_interval;
    static uint8_t fps_limit;

    static float delta_time;
    static uint8_t fps;
    static uint64_t fps_elapsed_ms;

    static void set_fps(uint8_t new_fps);

    static void run();
    static bool ticks_running_asynchronous();
    static void sync_running_ticks();
};

/**
 * The utilities of program.
 **/
namespace util {
    void log(const std::string &name);
};

#endif