#ifndef AGK_API_WORLD_CAMERA_H
#define AGK_API_WORLD_CAMERA_H

#include "api/world/camera/frustum.hpp"

class camera : public frustum {
protected:
    float mouse_sensitivity {};
    bool mouse_locked {};
    bool mouse_shown {};

    void update_rotation();
public:
    void set_mouse_sensitivity(float value);
    float get_mouse_sensitivity();

    void on_event(SDL_Event &sdl_event) override;
};

#endif