#ifndef AGK_API_WORLD_MATERIAL_H
#define AGK_API_WORLD_MATERIAL_H

#include <iostream>

namespace material {
    enum class composition {
        metal = 2,
        opaque = 4,
        light = 8
    };

    struct data {
    public:
        material::composition composition {};
        data(material::composition);
    };

    struct solid : public data {
    public:
        float color[3] {1, 0, 1};
    };

    struct light : public data {
    public:
        float intensity[3] {1, 1, 1};
        bool incoming {};
    };
}

#endif