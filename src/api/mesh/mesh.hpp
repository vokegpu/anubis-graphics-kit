#ifndef AGK_API_MESH_H
#define AGK_API_MESH_H

#include <vector>

namespace mesh {
    enum class format {
        unknown, obj, stl
    };

    struct data {
        mesh::format format {};
        int32_t vert_amount {};
        std::vector<float> vertices {};
        std::vector<float> texture_coordinates {};
        std::vector<float> normals {};
    };
}

#endif