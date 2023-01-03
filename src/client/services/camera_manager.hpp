#ifndef AGK_CLIENT_SERVICES_CAMERA_MANAGER_H
#define AGK_CLIENT_SERVICES_CAMERA_MANAGER_H

#include "api/feature/feature.hpp"
#include "api/world/camera/camera.hpp"

class camera_manager : public feature {
public:
    camera *p_camera_linked {};
    bool enable_movement {};
    uint8_t camera_rotate {};

    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
};

#endif