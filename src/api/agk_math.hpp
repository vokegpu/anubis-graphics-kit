#pragma once

#ifndef AGK_MATH_H
#define AGK_MATH_H

struct math {
    struct vec4f {
        float x;
        float y;
        float z;
        float w;

        vec4f();
        vec4f(float x, float y, float z, float w);
    };
};

#endif