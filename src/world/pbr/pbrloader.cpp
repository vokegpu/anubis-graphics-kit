#include "pbrloader.hpp"
#include "mehs/mesh.hpp"
#include "agk/agk.hpp"

int32_t pbrloader::find_equals_material(std::map<std::string, std::string> &metadata) {
    for (uint64_t it {}; it < this->material_list.size(); it++) {
        auto &p_material {this->material_list[it]};
        auto &material_metadata {p_material->get_metadata()};
    
        if (metadata["rough"] == metadata[''])
    }

    return false;
}

bool pbrloader::load_material(std::vector<std::string> &material_list, std::string_view path) {
    mesh::data data {};
    if (agk::mesh::loader()->load_material(data, path)) {
        return util::log("Failed to create material");
    }

    std::string key {}, value {};
    auto &metadata {data.pbr_serializer.get_metadata()};

    for (auto it {metadata.begin()}; it != metadata.end(); it = std::next(it)) {
        key = it->first;

        for (auto it_mtl {it->second.begin()}; it_mtl != it->second.end(); it_mtl = std::next(it_mtl)) {
            key = it_mtl->first;
            value = it_mtl->second;
        }
    }
}

void pbrloader::registry(model *p_model) {
    if (this->element_map[p_model->tag] == nullptr) {
        this->element_map[p_model->tag] = p_model;
    }
}

void pbrloader::registry(material *p_material) {
    if (this->element_map[p_material->tag] == nullptr) {
        this->element_map[p_material->tag] = p_material;
        this->material_list.push_back(p_material);
    }
}

material *pbrloader::find_material(std::string_view k_name) {
    return nullptr;
}

model *pbrloader::find_model(std::string_view k_name) {
    return nullptr;
}