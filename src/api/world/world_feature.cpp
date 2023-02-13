#include "world_feature.hpp"
#include "api/event/event.hpp"

void world_feature::on_low_update() {

}

void world_feature::set_visible(enums::state enum_state, bool dispatch_event) {
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

enums::state world_feature::get_visible() {
    return this->visible;
}

void world_feature::set_priority(enums::priority enum_priority, bool dispatch_event) {
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

enums::priority world_feature::get_priority() {
    return this->priority;
}
