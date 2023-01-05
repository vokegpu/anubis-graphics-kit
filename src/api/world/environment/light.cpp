#include "light.hpp"
#include "api/api.hpp"
#include "api/event/event.hpp"
#include "api/util/env.hpp"

light::light(model *p_model_linked) {
    this->p_model = p_model_linked;
    this->id = feature::token++;

    this->set_visible(enums::state::disable);
    this->set_priority(enums::priority::low);
}

light::~light() {

}

void light::on_low_update() {
    shading::program *p_program_material_pbr {};
    api::shading::find("MaterialPBR", p_program_material_pbr);

    std::string light_index_tag {"Light["};
    light_index_tag += std::to_string(this->index_light);
    light_index_tag += ']';

    glUseProgram(p_program_material_pbr->id);
    p_program_material_pbr->set_uniform_vec3(light_index_tag + ".Intensity", &this->intensity[0]);
    p_program_material_pbr->set_uniform_bool(light_index_tag + ".Directional", this->directional);
    p_program_material_pbr->set_uniform_vec3(light_index_tag + ".POD", &this->position[0]);
    glUseProgram(0);

    this->low_update_ticking = false;
}

void light::update() {
    if (!this->low_update_ticking) {
        this->set_priority(enums::priority::low);
    }

    this->low_update_ticking = true;
}