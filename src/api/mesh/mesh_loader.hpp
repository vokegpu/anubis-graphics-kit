#ifndef AGK_API_MESH_LOADER_H
#define AGK_API_MESH_LOADER_H

#include <string>
#include "mesh.hpp"
#include <map>

class mesh_loader {
protected:
    std::vector<mesh::vertex> vertex_positions_list {}, vertex_texture_coordinates_list {}, vertex_normals_list {};
    std::map<uint32_t, bool> repeated_indexes {};
public:
    void process_indexing(mesh::data&);
    bool load_object(mesh::data&, std::string_view);
};

#endif