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
        float color[3] {1, 0, 1};
        data(material::composition);
    };

    struct light : public data {
    public:
        bool physically_accurate {true};
        float intensity[3] {1, 1, 1};
        int32_t shininess {32};
        bool incoming {};
    };
}

#endif