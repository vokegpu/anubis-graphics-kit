#ifndef AGK_CLIENT_SERVICES_CAMERA_MANAGER_H
#define AGK_CLIENT_SERVICES_CAMERA_MANAGER_H

#include "api/feature/feature.hpp"
#include "api/world/camera/camera.hpp"
#include "api/value/value.hpp"
#include <iostream>

class camera_manager : public feature {
protected:
    bool camera_rotation {};
    bool camera_movement {};
    bool camera_editor {};
public:
    camera *p_camera_linked {};

    void set_editor_enabled(bool enabled);
    bool is_editor_enabled();

    void set_movement_enabled(bool enabled);
    bool is_movement_enabled();

    void set_rotation_enabled(bool enabled);
    bool is_rotation_enabled();

    value<std::string> setting_m_forward {};
    value<std::string> setting_m_backward {};
    value<std::string> setting_m_strafe_left {};
    value<std::string> setting_m_strafe_right {};
    value<std::string> setting_editor_rotate {};

    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
};

#endif