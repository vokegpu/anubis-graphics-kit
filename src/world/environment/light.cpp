#include "light.hpp"
#include "agk.hpp"
#include "event/event.hpp"
#include "util/env.hpp"

light::light() {
    this->id = imodule::token++;
    this->type = enums::type::light;

    this->set_visible(enums::state::disable);
    this->set_priority(enums::priority::low);
}

light::~light() {

}

void light::on_low_update() {
    ::asset::shader *p_program_pbr {(::asset::shader*) agk::asset::find("gpu/effects.material.brdf.pbr")};

    std::string light_index_tag {"uLight["};
    light_index_tag += std::to_string(this->index);
    light_index_tag += ']';

    p_program_pbr->invoke();
    p_program_pbr->set_uniform_vec3(light_index_tag + ".uIntensity", &this->intensity[0]);
    p_program_pbr->set_uniform_bool(light_index_tag + ".uDirectional", this->directional);
    p_program_pbr->set_uniform_vec3(light_index_tag + ".uVector", &this->transform.position[0]);
    glUseProgram(0);

    p_program_pbr = (::asset::shader*) agk::asset::find("gpu/effects.terrain.pbr");
    light_index_tag = "uLight[";
    light_index_tag += std::to_string(this->index);
    light_index_tag += ']';

    p_program_pbr->invoke();
    p_program_pbr->set_uniform_vec3(light_index_tag + ".uIntensity", &this->intensity[0]);
    p_program_pbr->set_uniform_bool(light_index_tag + ".uDirectional", this->directional);
    p_program_pbr->set_uniform_vec3(light_index_tag + ".uVector", &this->transform.position[0]);
    glUseProgram(0);

    this->low_update_ticking = false;
}

void light::update() {
    if (!this->low_update_ticking) {
        this->set_priority(enums::priority::low);
    }

    this->low_update_ticking = true;
}
