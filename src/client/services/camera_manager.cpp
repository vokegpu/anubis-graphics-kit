#include "camera_manager.hpp"
#include "api/api.hpp"

void client::services::camera_manager::on_create() {
    feature::on_create();

    this->setting_m_backward.set_value("s");
    this->setting_m_forward.set_value("w");
    this->setting_m_strafe_left.set_value("a");
    this->setting_m_strafe_right.set_value("d");
    this->setting_editor_rotate.set_value("mouse-0");
    this->p_camera_linked = api::world::currentcamera();
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
        this->camera_rotation && !this->camera_editor || (this->camera_editor && api::input::pressed(this->setting_editor_rotate.get_value()))
    };

    this->p_camera_linked->set_mouse_locked(should_rotate);
    this->p_camera_linked->on_event(sdl_event);
}

void client::services::camera_manager::on_update() {
    feature::on_update();
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
