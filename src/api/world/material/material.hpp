#ifndef AGK_API_WORLD_MATERIAL_H
#define AGK_API_WORLD_MATERIAL_H

namespace material {
    enum composition {
        metal = 2,
        opaque = 4
    };

    enum lighting {
        diffuse = 8,
        specular = 16
    };
}

#endif