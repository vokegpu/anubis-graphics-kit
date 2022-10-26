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
        std::vector<float> vertices {}, texture_coordinates {}, normals {};
        std::vector<uint32_t> vertices_index {}, texture_coordinates_index {}, normals_index {};
    };
}

#endif