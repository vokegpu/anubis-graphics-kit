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

bool object::contains_assign(std::string_view model, std::string_view material) {
    std::string ref_tag {model}; ref_tag += material;
    if (this->ref_object_assign_map[ref_tag.data()] != nullptr) {
        return true;
    }

    for (objectassign &object_assign : this->object_assign_list) {
        if (object_assign.p_linked_model != nullptr && object_assign.p_linked_model->tag == model && (material.empty() || (object_assign.p_linked_material != nullptr && object_assign.p_linked_material->tag == material))) {
            return true;
        }
    }

    return false;
}

void object::assign(std::string_view model, std::string_view material) {
    if (model.empty() || material.empty()) {
        return;
    }

    std::string ref_tag {};
    ref_tag += model;
    ref_tag += material;

    if (!this->contains_assign(model)) {
        return;
    }

    auto *p_material {(::material*) agk::pbr::find(material)};
    auto *p_model {(::model*) agk::pbr::find(model)};

    if (p_material == nullptr || p_model == nullptr) {
        return;
    }

    auto &object_assign {this->object_assign_list.emplace_back()};
    object_assign.p_linked_model = p_model;
    object_assign.p_linked_material = p_material;
    this->ref_object_assign_map[ref_tag] = &object_assign;
}

void object::on_low_update() {

}

void object::set_visible(enums::state enum_state, bool dispatch_event) {
    if (this->visible != enum_state && dispatch_event) {
        int32_t *p_id {new int32_t(this->id)};
        bool *p_boolean {new bool(enum_state == enums::state::disable)};

        SDL_Event sdl_event {};
        sdl_event.user.data1 = p_boolean;
        sdl_event.user.data2 = p_id;

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
        int32_t *p_id {new int32_t(this->id)};
        bool *p_boolean {new bool(enum_priority == enums::priority::high)};

        SDL_Event sdl_event {};
        sdl_event.user.data1 = p_boolean;
        sdl_event.user.data2 = p_id;

        event::dispatch(sdl_event, event::type::world_changed_priority);
    } else {
        this->priority = enum_priority;
    }
}

enums::priority object::get_priority() {
    return this->priority;
}

object::object() {
    this->id = imodule::token++;
    this->set_priority(enums::priority::high);
    this->set_visible(enums::state::enable);
}

object::~object() {

}