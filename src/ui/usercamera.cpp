#include "usercamera.hpp"
#include "agk.hpp"
#include "util/env.hpp"

void usercamera::on_create() {
    imodule::on_create();

    this->bind_m_backward.set_value("s");
    this->bind_m_forward.set_value("w");
    this->bind_m_strafe_left.set_value("a");
    this->bind_m_strafe_right.set_value("d");
    this->bind_editor_rotate.set_value("mouse-1");
    this->bind_m_crouch.set_value("c");
    this->bind_m_jump.set_value("space");
    this->m_speed.set_value(0.2867f);

    this->p_camera_linked = agk::world::current_camera();
    this->p_entity_linked = agk::world::current_player();
    this->p_camera_linked->set_visible(enums::state::disable);
}

void usercamera::on_destroy() {
    imodule::on_destroy();
}

void usercamera::on_event(SDL_Event &sdl_event) {
    imodule::on_event(sdl_event);
    if (this->p_camera_linked == nullptr) {
        return;
    }

    bool should_rotate {
        this->camera_rotation && (!this->camera_editor || agk::ui::input(this->bind_editor_rotate.get_value()))
    };

    this->p_camera_linked->set_mouse_locked(should_rotate);
    this->p_camera_linked->on_event(sdl_event);
}

void usercamera::on_update() {
    imodule::on_update();

    if (this->p_entity_linked != nullptr && this->camera_movement) {
        glm::vec3 motion {};
        bool rotated {};

        if (agk::ui::input(this->bind_m_forward.get_value())) {
            motion.z += 1;
        }

        if (agk::ui::input(this->bind_m_backward.get_value())) {
            motion.z -= 1;
        }

        if (agk::ui::input(this->bind_m_strafe_left.get_value())) {
            motion.x += 1;
        }

        if (agk::ui::input(this->bind_m_strafe_right.get_value())) {
            motion.x -= 1;
        }

        if (agk::ui::input(this->bind_m_jump.get_value())) {
            motion.y += 1;
        }

        if (agk::ui::input(this->bind_m_crouch.get_value())) {
            motion.y -= 1;
        }

        if (agk::ui::input("left")) {
            this->p_camera_linked->transform.rotation.y -= 0.9f;
            rotated = true;
        }

        if (agk::ui::input("right")) {
            this->p_camera_linked->transform.rotation.y += 0.9f;
            rotated = true;
        }

        if (agk::ui::input("up")) {
            this->p_camera_linked->transform.rotation.x += 0.9f;
            rotated = true;
        }

        if (agk::ui::input("down")) {
            this->p_camera_linked->transform.rotation.x -= 0.9f;
            rotated = true;
        }

        float yaw {p_camera_linked->transform.rotation.y};
        float x {glm::cos(glm::radians(yaw))};
        float z {glm::sin(glm::radians(yaw))};

        this->m_speed.set_value(this->p_entity_linked->speed_base);
        float speed {this->m_speed.get_value()};

        this->p_entity_linked->velocity.x = motion.z * speed * x + motion.x * speed * z;
        this->p_entity_linked->velocity.z = motion.z * speed * z - motion.x * speed * x;
        this->p_entity_linked->velocity.y = motion.y * speed;

        this->p_entity_linked->transform.rotation = this->p_camera_linked->transform.rotation;
        this->p_camera_linked->transform.position = this->p_entity_linked->transform.position;
        
        if (rotated) {
            this->p_camera_linked->update_rotation();
        }
    }
}

void usercamera::set_editor_enabled(bool enabled) {
    this->camera_editor = enabled;
}

bool usercamera::is_editor_enabled() {
    return this->camera_editor;
}

void usercamera::set_movement_enabled(bool enabled) {
    this->camera_movement = enabled;
}

bool usercamera::is_movement_enabled() {
    return this->camera_movement;
}

void usercamera::set_rotation_enabled(bool enabled) {
    this->camera_rotation = enabled;
}

bool usercamera::is_rotation_enabled() {
    return this->camera_rotation;
}