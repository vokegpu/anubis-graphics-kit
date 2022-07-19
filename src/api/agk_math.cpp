#include "agk_math.hpp"

math::vec4f::vec4f() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->w = 0;
}

math::vec4f::vec4f(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}
