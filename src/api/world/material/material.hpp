#ifndef AGK_API_WORLD_MATERIAL_H
#define AGK_API_WORLD_MATERIAL_H

#include <iostream>
#include <glm/glm.hpp>

namespace material {
    enum class composition {
        metal = 2,
        opaque = 4,
        light = 8
    };

    enum class lighting {
        blinn, pbr
    };

    struct data {
    public:
        material::composition composition {};
        data(material::composition);
    };

    struct solid : public data {
    public:
        glm::vec3 color {1, 0, 1};
        float rough {0.47f};
    
        glm::vec3 ka {0.4f, 0.4f, 0.4f}, kd {0.5f, 0.5f, 0.5f}, ks {0.5f, 0.5f, 0.5f};
        int shininess {16};
    };

    struct light : public data {
    public:
        glm::vec3 intensity {1, 1, 1};
        glm::vec3 la {0.2f, 0.2f, 0.2f};
        glm::vec3 ld {0.2f, 0.2f, 0.2f};
        glm::vec3 ls {0.5f, 0.5f, 0.5f};

        bool incoming {};
        material::lighting lighting {};
    };
}

#endif