#include "pbrloader.hpp"
#include "mehs/mesh.hpp"
#include "agk/agk.hpp"

std::vector<std::string> pbrloader::dontcare {};

int32_t pbrloader::find_equals_material(std::map<std::string, std::string> &metadata) {
    for (uint64_t it {}; it < this->material_list.size(); it++) {
        auto &p_material {this->material_list[it]};
        auto &mtl_metadata {p_material->get_metadata()};
    
        if (metadata["rough"] == mtl_metadata["rough"]) {
            return it;
        }
    }

    return -1;
}

bool pbrloader::load_model(std::string_view tag, std::vector<std::string> &loaded_model_list, std::string_view path) {
    stream::format model_format {agk::core.parser.get_model_format(path)};
    std::vector<stream::mesh> meshes {};

    switch (model_format) {
    case stream::format::gltf:
        if (agk::core.parser.load_gltf_meshes(meshes, path)) {
            return true;
        }
        break;
    default:
        auto &mesh {meshes.emplace_back()};
        if (agk::core.parser.load_mesh(mesh, path)) {
            return true;
        }
        break;
    }

    /*
     * A concatenate can be performed if the mesh stream come from a glTF (nodes),
     * wavefront objects and STL do not need to split into different models tag.
     */

    std::string ref_model_tag {};
    std::string model_tag {name};

    for (stream::mesh &mesh : meshes) {
        ref_model_tag = model_tag + mesh.tag;
        if (this->model_map[ref_model_tag] != nullptr) {
            continue;
        }

        model *p_model {new model {mesh}};
        p_model->tag = ref_model_tag;

        this->uncompiled_model_list.push_back(p_model);
        this->model_map[ref_model_tag] = p_model;
    }
}

bool pbrloader::load_material(std::vector<std::string> &loaded_material_list, std::string_view path) {
    stream::mtl mtl {};
    if (agk::core.parser.load_material(mtl, path)) {
        return true;
    }

    std::string key {}, value {};
    auto &metadata {mtl.get_serializer().get_metadata()};

    for (auto it {metadata.begin()}; it != metadata.end(); it = std::next(it)) {
        key = it->first;
        if (this->material_map.count(key)) {
            continue;
        }

        material *p_material {new material {it->second}};
        p_material->tag = key;
        loaded_material_list.push_back(key);
        this->material_map[key] = p_material;

        // @TODO check duplicated materials (rough, metal factor, textures)
    }

    return false;
}

material *&pbrloader::find_material(std::string_view k_name) {
    return this->material_map[k_name.data()];
}

model *&pbrloader::find_model(std::string_view k_name) {
    return this->model_map[k_name.data()];
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