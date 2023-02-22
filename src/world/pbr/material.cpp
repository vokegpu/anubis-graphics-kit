#include "material.hpp"

material::material(enums::material material_type) {
    this->type = material_type;
}

material::~material() = default;

enums::material material::get_type() {
    return this->type;
}

void material::set_texture(int32_t id) {
    this->texture = id;
}

int32_t material::get_texture() {
    return this->texture;
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