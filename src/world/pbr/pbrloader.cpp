#include "pbrloader.hpp"
#include "stream/stream.hpp"
#include "agk.hpp"
#include "util/env.hpp"

std::vector<std::string> pbrloader::dontcare {};

int32_t pbrloader::find_equals_material(std::map<std::string, std::string> &metadata) {
    return -1;
}

int32_t pbrloader::extract_family_tag(std::string_view pbr, std::string &family_tag) {
    pbrloader::dontcare.resize(4);
    util::split(pbrloader::dontcare, pbr, '.');

    family_tag += pbrloader::dontcare[0];
    family_tag += '.';
    family_tag += pbrloader::dontcare[1];

    return pbrloader::dontcare[2].empty() ? -1 : std::stoi(pbrloader::dontcare[2]);
}

bool pbrloader::load_model(std::string_view tag, std::string_view path) {
    std::string id_tag {"model."}; id_tag += tag;
    std::vector<stream::mesh> meshes {};

    if (agk::app.parser.load_meshes(meshes, path)) {
        return true;
    }

    /*
     * A concatenated can be performed if the mesh stream come from a glTF (nodes),
     * wavefront objects and STL do not need to split into different models tag.
     */

    std::string ref_model_tag {};
    for (stream::mesh &mesh : meshes) {
        ref_model_tag = id_tag + "." + mesh.tag;
        if (this->pbr_map[ref_model_tag] != nullptr) {
            continue;
        }

        model *p_model {new model {mesh}};
        p_model->tag = ref_model_tag;

        this->uncompiled_model_list.push_back(p_model);
        this->pbr_map[ref_model_tag] = p_model;
        util::log("PBR model loaded '" + p_model->tag + "'");
    }

    this->pbr_family_map.insert({id_tag, meshes.size()});
    return false;
}

bool pbrloader::load_material(std::string_view tag, material *p_material) {
    if (p_material == nullptr) {
        return true;
    }

    // Repeating family names repeat the couter
    std::string id_tag {"material."};
    id_tag += tag;
    id_tag += '.';
    id_tag += std::to_string(this->pbr_family_map[id_tag.data()]++);

    if (this->pbr_map.count(id_tag)) {
        return true;
    }

    this->pbr_map[id_tag] = p_material;
    p_material->tag = id_tag;

    return !util::log("PBR material loaded '" + p_material->tag + "'");
}

bool pbrloader::load_material(std::string_view tag, std::string_view path) {
    stream::mtl mtl {};
    if (agk::app.parser.load_mtl(mtl, path)) {
        return true;
    }

    std::string key {}, value {};
    auto &metadata {mtl.get_serializer().get_metadata()};

    for (auto it {metadata.begin()}; it != metadata.end(); it = std::next(it)) {
        key = "material.";
        key += tag;
        key += '.';
        key += it->first;

        if (this->pbr_map.count(key)) {
            continue;
        }

        material *p_material {new material {it->second}};
        p_material->tag = key;
        this->pbr_map[key] = p_material;

        util::log("PBR material loaded '" + p_material->tag + "'");
        // @TODO check duplicated materials (rough, metal factor, textures)
    }

    this->pbr_family_map.insert({});
    return false;
}

imodule *pbrloader::find(std::string_view pbr_tag) {
    return this->pbr_map[pbr_tag.data()];
}

uint32_t pbrloader::find_family(std::string_view pbr_tag) {
    return this->pbr_family_map.count(pbr_tag.data()) ? this->pbr_family_map[pbr_tag.data()] : 0;
}

void pbrloader::direct_assign_object(object *p_object, std::string_view model, std::string_view material) {
    if (p_object->contains_assign(model, material)) {
        return;
    }

    auto *p_material {(::material*) this->find(material)};
    auto *p_model {(::model*) this->find(model)};

    if (p_material == nullptr || p_model == nullptr) {
        return;
    }

    auto &object_assign {p_object->find_assign_or_emplace_back(model, material)};
    object_assign.p_linked_model = p_model;
    object_assign.p_linked_material = p_material;
    p_object->update_aabb_checker();
}

void pbrloader::assign_object(object *p_object, std::string_view model, std::string_view material) {
    std::string model_id_tag {};
    std::string material_id_tag {};

    int32_t is_model_family {this->extract_family_tag(model, model_id_tag)};
    int32_t is_material_family {this->extract_family_tag(material, material_id_tag)};

    if (is_model_family == -1 && is_material_family == -1) {
        const uint32_t model_family {this->find_family(model_id_tag)};
        const uint32_t material_family {this->find_family(material_id_tag)};
        
        uint32_t model_it {};
        uint32_t material_it {};
        ::material *p_material {};

        for (model_it = 0; model_it < model_family; model_it++) {
            auto *p_model {(::model*) this->find(model_id_tag + "." + std::to_string(model_it))};
            if (p_model != nullptr) {
                p_material = (::material*) this->find(material_id_tag + "." + std::to_string(p_model->mesh.material));
            }

            if (p_material == nullptr) {
                p_material = (::material*) this->find(material_id_tag + "." + std::to_string(material_it));
            }

            if ((p_material == nullptr || p_model == nullptr) || p_object->contains_assign(p_model->tag, p_material->tag)) {
                continue;
            }

            auto &object_assign {p_object->find_assign_or_emplace_back(p_model->tag, p_material->tag)};
            object_assign.p_linked_model = p_model;
            object_assign.p_linked_material = p_material;
            p_object->update_aabb_checker();

            if (material_it < material_family) {
                material_it++;
            }
        }
    } else if (is_model_family != -1 && is_material_family == -1) {
        this->direct_assign_object(p_object, model_id_tag + "." + std::to_string(is_model_family), material_id_tag + ".0");
    } else if (is_model_family == -1 && is_material_family != -1) {
        const uint32_t model_family {this->find_family(model_id_tag)};

        for (uint32_t model_it {}; model_it < model_family; model_it++) {
            this->direct_assign_object(p_object, model_id_tag + "." + std::to_string(model_it), material_id_tag + "." + std::to_string(is_material_family));
        }
    } else {
        this->direct_assign_object(p_object, model_id_tag, material_id_tag);
    }
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