#include "material.hpp"
#include "agk.hpp"

material::material(enums::material material_type) {
    this->type = material_type;
}

material::~material() = default;

enums::material material::get_type() {
    return this->type;
}

void material::set_color(glm::vec3 color3f) {
    this->color = color3f;
}

glm::vec3 material::get_color() {
    return this->color;
}

void material::set_rough_based_type(enums::material material_type) {
    this->set_rough(material_type == enums::material::metal ? 0.43f : 1.0f);
}

void material::set_rough(float rough_value) {
    this->rough = rough_value;
}

float material::get_rough() {
    return this->rough;
}

void material::append_texture(uint32_t texture) {
    bool contains {};
    for (uint32_t &textures : this->texture_list) {
        contains = textures == texture;
        if (contains) {
            break;
        }
    }

    if (!contains) {
        this->texture_list.push_back(texture);
    }
}

void material::remove_texture(uint32_t texture) {
    int64_t index {-1};
    for (uint64_t it {}; it < this->texture_list.size(); it++) {
        if (this->texture_list[it] == texture) {
            index = (int64_t) it;
            break;
        }
    }

    if (index != -1) {
        this->texture_list.erase(this->texture_list.begin() + index);
    }
}

bool material::invoke_all_textures() {
    if (!this->material_mtl_updated) {
        this->material_mtl_updated = true;

        ::asset::model *p_asset_model {(::asset::model*) agk::asset::find(this->asset_model_mtl)};
        ::asset::texture<uint8_t> *p_asset_texture {};

        for (std::string &mtl : p_asset_model->get_linked_mtl_list()) {
            p_asset_texture = (::asset::texture<uint8_t>*) agk::asset::find(mtl);
            if (p_asset_texture != nullptr) {
                this->append_texture(p_asset_texture);
            }
        }
    }

    if (this->texture_list.empty()) {
        return false;
    }

    // @TODO Fix multi-active samplers in only one material
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture_list[0]);

    return true;
}

void material::append_mtl(::asset::model *p_asset_model) {
    if (p_asset_model == nullptr) {
        return;
    }

    this->asset_model_mtl = p_asset_model->tag;
    this->material_mtl_updated = false;
}