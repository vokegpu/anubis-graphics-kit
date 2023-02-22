#ifndef AGK_ASSET_MESH_LOADER_H
#define AGK_ASSET_MESH_LOADER_H

#include <string>
#include "mesh.hpp"
#include "util/env.hpp"
#include "util/file.hpp"
#include <map>
#include <glm/glm.hpp>
#include <unordered_map>

class mesh_loader {
protected:
    std::vector<glm::vec3> v_packed_list {}, t_packed_list {}, n_packed_list {};
    std::map<uint32_t, bool> repeated_indexes {};
    std::map<std::string, mesh::format> mesh_ext_map {
            {"stl", mesh::format::stl}, {"obj", mesh::format::obj}
    };

    void load_wavefront_object(mesh::data &data, std::ifstream &ifs);
    void load_stl_object(mesh::data &data, std::ifstream &ifs);
public:
    void process_indexing(mesh::data &data);
    bool load_object(mesh::data &data, std::string_view path);
    bool load_heightmap(mesh::data &data, util::image &resource);
    bool load_identity_heightmap(mesh::data &data, uint32_t width, uint32_t height);
};

#endif