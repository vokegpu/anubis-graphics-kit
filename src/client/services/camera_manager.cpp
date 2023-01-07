#include "camera_manager.hpp"
#include "api/api.hpp"
#include "api/util/env.hpp"

void client::services::camera_manager::on_create() {
    feature::on_create();

    this->bind_m_backward.set_value("s");
    this->bind_m_forward.set_value("w");
    this->bind_m_strafe_left.set_value("a");
    this->bind_m_strafe_right.set_value("d");
    this->bind_editor_rotate.set_value("mouse-1");
    this->bind_m_crouch.set_value("c");
    this->bind_m_jump.set_value("space");
    this->m_speed.set_value(0.2867f);

    this->p_camera_linked = api::world::currentcamera();
    this->p_entity_linked = api::world::currentplayer();
    this->p_camera_linked->set_visible(enums::state::disable);
}

void client::services::camera_manager::on_destroy() {
    feature::on_destroy();
}

void client::services::camera_manager::on_event(SDL_Event &sdl_event) {
    feature::on_event(sdl_event);
    if (this->p_camera_linked == nullptr) {
        return;
    }

    bool should_rotate {
        this->camera_rotation && (!this->camera_editor || api::input::pressed(this->bind_editor_rotate.get_value()))
    };

    this->p_camera_linked->set_mouse_locked(should_rotate);
    this->p_camera_linked->on_event(sdl_event);
}

void client::services::camera_manager::on_update() {
    feature::on_update();

    if (this->p_entity_linked != nullptr && this->camera_movement) {
        glm::vec3 motion {};
        if (api::input::pressed(this->bind_m_forward.get_value())) {
            motion.z += 1;
        }

        if (api::input::pressed(this->bind_m_backward.get_value())) {
            motion.z -= 1;
        }

        if (api::input::pressed(this->bind_m_strafe_left.get_value())) {
            motion.x += 1;
        }

        if (api::input::pressed(this->bind_m_strafe_right.get_value())) {
            motion.x -= 1;
        }

        if (api::input::pressed(this->bind_m_jump.get_value())) {
            motion.y += 1;
        }

        if (api::input::pressed(this->bind_m_crouch.get_value())) {
            motion.y -= 1;
        }

        float yaw {p_camera_linked->rotation.x};
        float x {glm::cos(glm::radians(yaw))};
        float z {glm::sin(glm::radians(yaw))};

        this->m_speed.set_value(this->p_entity_linked->speed_base);
        float speed {this->m_speed.get_value()};

        this->p_entity_linked->velocity.x = motion.z * speed * x + motion.x * speed * z;
        this->p_entity_linked->velocity.z = motion.z * speed * z - motion.x * speed * x;
        this->p_entity_linked->velocity.y = motion.y * speed;

        this->p_entity_linked->rotation = this->p_camera_linked->rotation;
        this->p_camera_linked->position = this->p_entity_linked->position;
    }
}

void client::services::camera_manager::set_editor_enabled(bool enabled) {
    this->camera_editor = enabled;
}

bool client::services::camera_manager::is_editor_enabled() {
    return this->camera_editor;
}

void client::services::camera_manager::set_movement_enabled(bool enabled) {
    this->camera_movement = enabled;
}

bool client::services::camera_manager::is_movement_enabled() {
    return this->camera_movement;
}

void client::services::camera_manager::set_rotation_enabled(bool enabled) {
    this->camera_rotation = enabled;
}

bool client::services::camera_manager::is_rotation_enabled() {
    return this->camera_rotation;
}
