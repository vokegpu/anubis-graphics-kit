#include "material.hpp"
#include "agk.hpp"

int32_t material::token {};

material::material() {
    this->id = ++material::token;
    agk::world::renderer()->material_list.push_back(this);
    this->should_reload = true;
}

material::~material() {

}

material_metadata &material::getmetadata() {
    return this->metadata;
}

void material::set_is_metal(bool _is_metal) {
    this->metadata.metal = _is_metal;
    if (this->metadata.rough == 0.0f) {
        this->metadata.rough = 0.43f;
    }
}

bool material::is_metal() {
    return this->metadata.metal;
}

void material::set_rough(float _rough) {
    this->metadata.rough = _rough;
}

float material::get_rough() {
    return this->metadata.rough;
}

void material::set_color(const glm::vec3 &color) {
    this->metadata.color = color;
}

const glm::vec3 &material::get_color() {
    return this->metadata.color;
}

void material::invoke(::asset::shader *p_shader) {
    auto &shaderbuffer {p_shader->shaderbuffer};

    if (this->should_reload) {
        this->should_reload = false;
        this->shader_index = p_shader->find(this->id);

        if (this->shader_index == -1) {
            this->shader_index = p_shader->append(this->id);
        }

        float l_metadata[12] {
            static_cast<float>(this->metadata.active_sampler_albedo), static_cast<float>(this->metadata.active_sampler_specular), static_cast<float>(this->metadata.active_sampler_normal_map), 0.0f,
            static_cast<float>(this->metadata.metal), this->metadata.rough, 0.0f, 0.0f,
            this->metadata.color.x, this->metadata.color.y, this->metadata.color.z, 0.0f
        };

        int32_t size {static_cast<int32_t>(sizeof(l_metadata) * this->shader_index)};

        shaderbuffer.invoke(0, GL_UNIFORM_BUFFER);
        shaderbuffer.edit<float>(size, sizeof(l_metadata), l_metadata);
        shaderbuffer.revoke();
    }

    p_shader->set_uniform_int("uMaterialIndex", this->shader_index);

    auto &albedo {this->sampler_map["albedo"]};
    if (albedo) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo);
    }

    auto &specular {this->sampler_map["specular"]};
    if (specular) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular);
    }

    auto &normal_map {this->sampler_map["normalmap"]};
    if (normal_map) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normal_map);
    }
}