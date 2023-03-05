#ifndef AGK_ASSET_MESH_LOADER_MESH_PACK_H
#define AGK_ASSET_MESH_LOADER_MESH_PACK_H

#include <iostream>
#include <vector>
#include "mesh.hpp"
#include <glm/glm.hpp>

struct meshpack {
    std::vector<glm::vec3> v {};
    std::vector<glm::vec2> t {};
    std::vector<glm::vec3> n {};
};

#endif

#ifndef AGK_ASSET_MESH_LOADER_H
#define AGK_ASSET_MESH_LOADER_H

#include <string>
#include "util/env.hpp"
#include "util/file.hpp"
#include <map>
#include <unordered_map>
#include <nlohmann/json.hpp>

class mesh_loader {
protected:
    std::string current_path {};
    std::map<std::string, mesh::format> mesh_ext_map {
            {"stl", mesh::format::stl}, {"obj", mesh::format::wavefrontobj}, {"gltf", mesh::format::gltf}
    };

    void load_wavefront_object_mtllib(mesh::data &data, std::ifstream &ifs);
    void load_wavefront_object(mesh::data &data, std::ifstream &ifs);
    void load_stl_object(mesh::data &data, std::ifstream &ifs);
public:
    bool load_object(mesh::data &data, std::string_view path);
    bool load_heightmap(mesh::data &data, util::image &resource);
    bool load_identity_heightmap(mesh::data &data, uint32_t width, uint32_t height);
};

#endif