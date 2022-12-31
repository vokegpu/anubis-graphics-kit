#include "material.hpp"

pbrm::pbrm() {

}

pbrm::~pbrm() = default;

enums::material pbrm::get_type() {
	return this->type;
}

void pbrm::set_texture(int32_t id) {
	this->texture = id;
}

int32_t pbrm::get_texture() {
	return this->texture;
}

void pbrm::set_color(glm::vec3 color3f) {
	this->color = color3f;
}

glm::vec3 pbrm::get_color() {
	return this->color;
}

pbrm_light::pbrm_light() {
	this->type = enums::material::light;
}

pbrm_light::~pbrm_light() = default;

void pbrm_light::set_intensity(glm::vec3 color3f) {
	this->intensity = color3f;
}

glm::vec3 pbrm_light::get_intensity() {
	return this->intensity;
}

pbrm_solid::pbrm_solid(enums::material material_type) {
	this->set_rough_based_type(material_type);
}

pbrm_solid::~pbrm_solid() = default;

void pbrm_solid::set_rough_based_type(enums::material material_type) {
	this->set_rough(material_type == enums::material::metal ? 0.43f : 1.0f);
}

void pbrm_solid::set_rough(float rough_value) {
	this->rough = rough_value;
}

float pbrm_solid::get_rough() {
	return this->rough;
}