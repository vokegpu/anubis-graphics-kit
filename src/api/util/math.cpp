#include "math.hpp"

float util::clamp(float val, float min, float max) {
    return val > max ? max : (val < min ? min : val);
}

float util::min(float val, float min) {
    return val < min ? min : val;
}

float util::max(float val, float max) {
    return val > max ? max : val;
}
