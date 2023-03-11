#include "material.hpp"
#include "agk.hpp"

int32_t material::token {};

material::material(std::map<std::string, std::string> &_metadata) {
    this->id = ++material::token;
    agk::world::renderer()->material_list.push_back(this);
    this->should_reload = true;
    this->metadata = _metadata;
}

std::vector<std::string, std::string> &material::get_metadata() {
    return this->metadata;
}

void material::invoke(::asset::shader *p_shader) {
    if (this->should_reload_textures) {
        this->should_reload_textures = false;
        asset::texture<uint8_t> *p_texture {};

        if (!this->metadata["albedo"].empty() && (p_texture = (asset::texture<uint8_t>*) agk::asset::find(this->metadata["albedo"])) != nullptr) {
            this->sampler_map["albedo"] = p_texture->get_gpu_side().id;
        }

        if (!this->metadata["specular"].empty() && (p_texture = (asset::texture<uint8_t>*) agk::asset::find(this->metadata["specular"])) != nullptr) {
            this->sampler_map["specular"] = p_texture->get_gpu_side().id;
        }

        if (!this->metadata["normalMap"].empty() && (p_texture = (asset::texture<uint8_t>*) agk::asset::find(this->metadata["normalMap"])) != nullptr) {
            this->sampler_map["normalMap"] = p_texture->get_gpu_side().id;
        }
    }

    auto &shaderbuffer {p_shader->shaderbuffer};
    auto &albedo {this->sampler_map["albedo"]};
    auto &specular {this->sampler_map["specular"]};
    auto &normal_map {this->sampler_map["normalMap"]};

    if (this->should_reload) {
        this->should_reload = false;
        this->shader_index = p_shader->find(this->id);

        if (this->shader_index == -1) {
            this->shader_index = p_shader->append(this->id);
        }

        std::vector<std::string> color_split {};
        util::split(color_split, this->metadata["color"], ' ');

        const float rough {std::stof(this->metadata["rough"])};
        const float is_metal {std::stof(this->metadata["metal"])};
        const glm::vec3 color {std::stof(color_split[0]), std::stof(color_split[1]), std::stof(color_split[2])};

        float buffer_database[12] {
            static_cast<float>(!(!(albedo))), static_cast<float>(!(!(specular))), static_cast<float>(!(!(normal_map))), 0.0f,
            is_metal, rough, 0.0f, 0.0f,
            color.x, color.y, color.z, 0.0f
        };

        int32_t size {static_cast<int32_t>(sizeof(buffer_database)*this->shader_index)};
        shaderbuffer.invoke(0, GL_UNIFORM_BUFFER);
        shaderbuffer.edit<float>(size, sizeof(buffer_database), buffer_database);
        shaderbuffer.revoke();
    }

    p_shader->set_uniform_int("uMaterialIndex", this->shader_index);
    int32_t face_cull_mode {};
    glGetIntegerv(GL_CULL_FACE_MODE, &face_culling_enable);
    int32_t face_culling_enable {glIsEnabled(GL_CULL_FACE)};

    if (this->double_sided && face_culling_enable) {
        glDisable(GL_CULL_FACE);
    } else if (face_cull_mode != GL_BACK) {
        if (!face_culling_enable) {
            glEnable(GL_ENABLE);
        }

        glCullFace(GL_BACK);
    }

    if (albedo) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo);
    }

    if (specular) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular);
    }

    if (normal_map) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normal_map);
    }
}