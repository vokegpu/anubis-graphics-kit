#include "object.hpp"
#include "event/event.hpp"
#include "agk.hpp"

void objecttransform::set_position(float x, float y, float z) {
    if (this->position.x == x && this->position.y == y && this->position.z == z) {
        return;
    }
}

void objecttransform::set_scale(float x, float y, float z) {

}

void object::on_low_update() {

}

void object::set_visible(enums::state enum_state, bool dispatch_event) {
    if (this->visible != enum_state && dispatch_event) {
        int32_t *p_wf_id {new int32_t(this->id)};
        bool *p_boolean {new bool(enum_state == enums::state::disable)};

        SDL_Event sdl_event {};
        sdl_event.user.data1 = p_boolean;
        sdl_event.user.data2 = p_wf_id;

        event::dispatch(sdl_event, event::type::world_refresh_environment);
    } else {
        this->visible = enum_state;
    }
}

enums::state object::get_visible() {
    return this->visible;
}

void object::set_priority(enums::priority enum_priority, bool dispatch_event) {
    if ((this->priority != enum_priority or enum_priority == enums::priority::low) && dispatch_event) {
        int32_t *p_wf_id {new int32_t(this->id)};
        bool *p_boolean {new bool(enum_priority == enums::priority::high)};

        SDL_Event sdl_event {};
        sdl_event.user.data1 = p_boolean;
        sdl_event.user.data2 = p_wf_id;

        event::dispatch(sdl_event, event::type::world_changed_priority);
    } else {
        this->priority = enum_priority;
    }
}

enums::priority object::get_priority() {
    return this->priority;
}

void object::on_render() {
    for (auto &rendering_pair : this->rendering_pair_list) {
        if (rendering_pair->p_material != nullptr) {
            rendering_pair->invoke(asset::shader::pcurrshader);
        }

        if (rendering_pair->p_model != nullptr) {
            rendering_pair->p_model->on_render();
        }
    }
}

object::object(material *p_linked_material) {
    this->id = imodule::token++;
    this->set_priority(enums::priority::high);
    this->set_visible(enums::state::enable);
    this->p_material = p_linked_material;
}

object::~object() {

}