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
    auto *p_program_pbr {(::asset::shader*) agk::asset::find("gpu/effects.pbr.material.lighting")};
    auto &program_buffer {p_program_pbr->programbuffer};

    float buffer[12] {
        this->transform.position.x, this->transform.position.y, this->transform.position.z, static_cast<float>(this->directional),
        this->intensity.x, this->intensity.y, this->intensity.z, 0.0f
    };

    uint64_t size_of_buffer {sizeof(buffer)};

    programbuffer.invoke();
    programbuffer.edit<float>(size_of_buffer * this->index, size_of_buffer, buffer);
    programbuffer.revoke();

    this->low_update_ticking = false;
}

void light::update() {
    if (!this->low_update_ticking) {
        this->set_priority(enums::priority::low);
    }

    this->low_update_ticking = true;
}
