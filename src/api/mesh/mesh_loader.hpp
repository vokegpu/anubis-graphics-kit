#ifndef AGK_API_MESH_LOADER_H
#define AGK_API_MESH_LOADER_H

#include <string>
#include "mesh.hpp"
#include <map>
#include <glm/glm.hpp>

class mesh_loader {
protected:
    std::vector<glm::vec3> v_packed_list {}, t_packed_list {}, n_packed_list {};
    std::map<uint32_t, bool> repeated_indexes {};

    void load_wavefront_object(mesh::data &data, std::ifstream &ifs);
    void load_stl_object(mesh::data &data, std::ifstream &ifs);
public:
    void process_indexing(mesh::data&);
    bool load_object(mesh::data&, std::string_view);

    // todo: Add stl loader files.
};

#endif