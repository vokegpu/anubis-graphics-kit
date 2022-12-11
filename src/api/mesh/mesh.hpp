#ifndef AGK_API_MESH_H
#define AGK_API_MESH_H

#include <vector>
#include <iostream>

namespace mesh {
    enum class format {
        unknown, obj, stl
    };

    struct data {
        mesh::format format {};
        int32_t faces_amount {};
        std::vector<float> vertices {}, texture_coordinates {}, normals {}, instances {};
        std::vector<uint32_t> vertices_index {}, texture_coordinates_index {}, normals_index {}, indexes {};
        bool contains_vertices {}, contains_normals {}, contains_texture_coordinates {};
    };
}

#endif