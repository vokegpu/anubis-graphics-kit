#include "agk_material.hpp"

material::color::color() {
    this->r = 0;
    this->g = 0;
    this->b = 0;
    this->a = 0;
}

material::color::color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    this->r = red;
    this->g = green;
    this->b = blue;
    this->a = alpha;
}
