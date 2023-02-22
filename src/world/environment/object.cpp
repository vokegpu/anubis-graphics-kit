#include "object.hpp"
#include "util/event.hpp"

void object::on_low_update() {

}

void object::set_visible(enums::state enum_state, bool dispatch_event) {
    if (this->visible != enum_state && dispatch_event) {
        int32_t *p_wf_id {new int32_t(this->id)};
        bool *p_boolean {new bool(enum_state == enums::state::disable)};

        SDL_Event sdl_event {};
        sdl_event.user.data1 = p_boolean;
        sdl_event.user.data2 = p_wf_id;

        event::dispatch(sdl_event, event::WORLD_REFRESH_ENVIRONMENT);
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

        event::dispatch(sdl_event, event::WORLD_CHANGED_PRIORITY);
    } else {
        this->priority = enum_priority;
    }
}

enums::priority object::get_priority() {
    return this->priority;
}

object::object(::asset::model *p_linked_model) {
    this->id = imodule::token++;
    this->set_priority(enums::priority::high);
    this->set_visible(enums::state::enable);
}

object::~object() {

}

void object_transform::set_position(float x, float y, float z) {
    if (this->position.x == x && this->position.y == y && this->position.z == z) {
        return;
    }
}

void object_transform::set_scale(float x, float y, float z) {

}