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
    

    
    p_program_pbr = (::asset::shader*) agk::asset::find("gpu/effects.terrain.pbr");
    this->low_update_ticking = false;
}

void light::update() {
    if (!this->low_update_ticking) {
        this->set_priority(enums::priority::low);
    }

    this->low_update_ticking = true;
}
