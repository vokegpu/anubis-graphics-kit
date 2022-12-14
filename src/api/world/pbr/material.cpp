#include "material.hpp"

pbrm::pbrm() {

}

pbrm::~pbrm() {

}

enums::material::type pbrm::get_type() {
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

pbrm_light::~pbrm_light() {

}

void pbrm_light::set_intensity(glm::vec3 color3f) {
	this->intensity = color3f;
}

glm::vec3 pbrm_light::get_intesity() {
	return this->intensity;
}

pbrm_solid::pbrm_solid(enums::material mtype) {
	this->set_rough_based_type(mtype);
}

pbrm_solid::~pbrm_solid() {
}

void pbrm_solid::set_rough_based_type(enums::material mtype) {
	this->set_rough(mtype == enums::material::dialetric ? 0.43f : 1.0f);
}

void pbrm_solid::set_rough(float rough_value) {
	this->rough = rough_value;
}

float pbrm_solid::get_rough() {
	return this->rough;
}