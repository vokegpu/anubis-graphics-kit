#ifndef AGK_API_WORLD_MATERIAL_H
#define AGK_API_WORLD_MATERIAL_H

#include <iostream>

namespace material {
    enum composition {
        metal = 2,
        opaque = 4,
        light = 8
    };

    struct data {
    public:
        material::composition composition {};
        float color[4] {};
    };

    struct light : public data {
    public:
        bool physically_based {true};
        float intensity[4] {};
        int32_t shininess {32};
    };
}

#endif