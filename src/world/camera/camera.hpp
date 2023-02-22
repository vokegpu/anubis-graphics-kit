#ifndef AGK_WORLD_CAMERA_H
#define AGK_WORLD_CAMERA_H

#include "frustum.hpp"

class camera : public frustum {
protected:
    float mouse_sensitivity {0.2f};
    bool mouse_locked {};
    bool mouse_shown {};

    void update_rotation();
public:
    camera() = default;
    ~camera() = default;

    void set_mouse_sensitivity(float value);
    float get_mouse_sensitivity();

    void set_mouse_locked(bool state);
    bool is_mouse_locked();

    void on_create() override;
    void on_event(SDL_Event &sdl_event) override;
};

#endif