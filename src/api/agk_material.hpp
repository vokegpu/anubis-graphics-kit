#pragma once

#ifndef AGK_MATERIAL_H
#define AGK_MATERIAL_H

#include <iostream>

struct material {
    struct color {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 0;

        color();
        color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
    };
};

#endif