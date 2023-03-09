#include "object.hpp"
#include "event/event.hpp"
#include "agk.hpp"

void object_transform::set_position(float x, float y, float z) {
    if (this->position.x == x && this->position.y == y && this->position.z == z) {
        return;
    }
}

void object_transform::set_scale(float x, float y, float z) {

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

void object::set_material(material *p_material) {
    for (asset::model::child &childs : this->model_children) {
        contains = childs->name == key;
        if (contains) {
            childs->p_material = p_material;
            break;
        }
    }
}

void object::assign_material(std::string_view key, material *p_material) {
    if (this->p_model != nullptr && this->p_model->buffer_map.count(key)) {
        bool contains {};
        for (asset::model::child &childs : this->model_children) {
            contains = childs->name == key;
            if (contains) {
                childs->p_material = p_material;
                break;
            }
        }

        if (!contains) {
            auto &child {this->model_children.emplace_back()};
            child->name = key;
            child->p_material = p_material;
        }
    }
}

void object::on_render() {
    if (model::)
}

object::object(material *p_linked_material) {
    this->id = imodule::token++;
    this->set_priority(enums::priority::high);
    this->set_visible(enums::state::enable);
    this->p_material = p_linked_material;
}

object::~object() {

}