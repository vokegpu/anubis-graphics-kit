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

    if (this->contains_assign(model)) {
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
    this->rendering_aabb_check = true;
}

util::aabb &object::get_current_aabb() {
    if (this->rendering_aabb_check) {
        stream::mesh final_mesh {};
        for (objectassign &object_assign : this->object_assign_list) {
            if (object_assign.p_linked_model != nullptr) {
                final_mesh += object_assign.p_linked_model->mesh;
            }
        }

        this->aabb = {};
        util::generateaabb(this->aabb, final_mesh);
        this->rendering_aabb_check = false;
    }

    return this->aabb;
}

void object::on_low_update() {

}

void object::on_render() {
    /*
     * Model-material assign can solve the issue with rendering glTF models,
     * glTF models contains different models (while wavefront you do not have).
     */
    for (objectassign &object_assign : this->object_assign_list) {
        if (object_assign.p_linked_material != nullptr) {
            object_assign.p_linked_material->invoke(asset::shader::pcurrshader);
        }

        if (object_assign.p_linked_model != nullptr) {
            object_assign.p_linked_model->buffer.invoke();
            object_assign.p_linked_model->buffer.draw();
            object_assign.p_linked_model->buffer.revoke();
        }
    }
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

object::object(enums::state visibility) {
    this->id = imodule::token++;
    this->set_priority(enums::priority::high);
    this->set_visible(visibility);
}

object::~object() {

}