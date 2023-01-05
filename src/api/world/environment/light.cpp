#include "light.hpp"
#include "api/api.hpp"
#include "api/event/event.hpp"
#include "api/util/env.hpp"

light::light(model *p_model_linked) {
    this->p_model = p_model_linked;
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
    p_program_material_pbr->set_uniform_vec3(light_index_tag + ".DirOrPos", &this->position[0]);
    glUseProgram(0);

    util::log("low priority ticking");
    this->low_update_ticking = false;
}

void light::update() {
    if (!this->low_update_ticking) {
        int32_t *p_wf_id {new int32_t(this->id)};

        SDL_Event sdl_event {};
        sdl_event.user.data1 = p_wf_id;
        event::dispatch(sdl_event, event::WORLD_REFRESH_LOW_PRIORITY);
    }

    this->low_update_ticking = true;
}
