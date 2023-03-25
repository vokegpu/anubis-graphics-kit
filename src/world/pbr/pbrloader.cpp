#include "pbrloader.hpp"
#include "stream/stream.hpp"
#include "agk.hpp"

std::vector<std::string> pbrloader::dontcare {};

int32_t pbrloader::find_equals_material(std::map<std::string, std::string> &metadata) {
    return -1;
}

bool pbrloader::load_model(std::string_view tag, std::vector<std::string> &loaded_model_list, std::string_view path) {
    if (!loaded_model_list.empty()) {
        loaded_model_list.clear();
    }

    std::string id_tag {"model."}; id_tag += tag;
    stream::format model_format {agk::app.parser.get_model_format(path)};
    std::vector<stream::mesh> meshes {};

    switch (model_format) {
    case stream::format::gltf:
        if (agk::app.parser.load_gltf_meshes(meshes, path)) {
            return true;
        }
        break;
    default:
        auto &mesh {meshes.emplace_back()};
        if (agk::app.parser.load_mesh(mesh, path)) {
            return true;
        }

        mesh.tag.clear();
        break;
    }

    /*
     * A concatenated can be performed if the mesh stream come from a glTF (nodes),
     * wavefront objects and STL do not need to split into different models tag.
     */

    std::string ref_model_tag {};
    for (stream::mesh &mesh : meshes) {
        ref_model_tag = id_tag + (mesh.tag.empty() ? "" : ("." + mesh.tag));
        if (this->pbr_map[ref_model_tag] != nullptr) {
            continue;
        }

        model *p_model {new model {mesh}};
        p_model->tag = ref_model_tag;

        this->uncompiled_model_list.push_back(p_model);
        this->pbr_map[ref_model_tag] = p_model;
        util::log("PBR model loaded '" + p_model->tag + "'");
    }

    return false;
}

bool pbrloader::load_material(std::string_view tag, material *p_material) {
    std::string id_tag {"material."}; id_tag += tag;
    if (this->pbr_map.count(id_tag) || p_material == nullptr) {
        return true;
    }

    this->pbr_map[id_tag] = p_material;
    p_material->tag = id_tag;
    return !util::log("PBR material loaded '" + p_material->tag + "'");
}

bool pbrloader::load_material(std::vector<std::string> &loaded_material_list, std::string_view path) {
    if (!loaded_material_list.empty()) {
        loaded_material_list.clear();
    }

    stream::mtl mtl {};
    if (agk::app.parser.load_mtl(mtl, path)) {
        return true;
    }

    std::string key {}, value {};
    auto &metadata {mtl.get_serializer().get_metadata()};

    for (auto it {metadata.begin()}; it != metadata.end(); it = std::next(it)) {
        key = "material." + it->first;
        if (this->pbr_map.count(key)) {
            continue;
        }

        material *p_material {new material {it->second}};
        p_material->tag = key;
        loaded_material_list.push_back(key);
        this->pbr_map[key] = p_material;

        // @TODO check duplicated materials (rough, metal factor, textures)
    }

    return false;
}

imodule *pbrloader::find(std::string_view pbr_tag) {
    return this->pbr_map[pbr_tag.data()];
}

void pbrloader::on_update() {
    if (!this->uncompiled_model_list.empty()) {
        for (model *&p_model : this->uncompiled_model_list) {
            if (p_model != nullptr) {
                p_model->recompile();
            }
        }

        this->uncompiled_model_list.clear();
    }
}