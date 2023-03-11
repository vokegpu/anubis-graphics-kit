#ifndef AGK_MESH_LOADER_MESH_PACK_H
#define AGK_MESH_LOADER_MESH_PACK_H

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

#ifndef AGK_MESH_LOADER_H
#define AGK_MESH_LOADER_H

#include <string>
#include "util/env.hpp"
#include "util/file.hpp"
#include <map>
#include <unordered_map>
#include <nlohmann/json.hpp>

class meshloader {
protected:
    std::string current_path {};
    std::map<std::string, mesh::format> mesh_ext_map {
            {"stl", mesh::format::stl}, {"obj", mesh::format::wavefrontobj}, {"gltf", mesh::format::gltf}
    };

    std::map<std::string, std::string> mtl_pbr_conversions_map {
        {"map_Ka", "ambientMap"}, {"map_Kd", "albedoMap"}, {"map_Ks", "specularMap"}
    };

    mesh::serializer material_serializer {};
    mesh::gltf gltf {};

    void load_wavefront_object_mtllib(mesh::data &data, std::ifstream &ifs);
    void load_wavefront_object(mesh::data &data, std::ifstream &ifs);
    void load_stl_object(mesh::data &data, std::ifstream &ifs);
    void check_format(mesh::format &format, std::string_view path);
public:
    mesh::serializer convert_wavefront_object_mtllib_to_pbr(mesh::serializer &serializer);

    bool load_model(mesh::data &data, std::string_view path);
    bool load_heightmap(mesh::data &data, util::image &resource);
    bool load_identity_heightmap(mesh::data &data, uint32_t width, uint32_t height);
};

#endif