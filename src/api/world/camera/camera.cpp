#include "camera.hpp"
#include "api/util/env.hpp"

void camera::set_mouse_sensitivity(float value) {
    this->mouse_sensitivity = value;
}

float camera::get_mouse_sensitivity() {
    return this->mouse_sensitivity;
}

void camera::on_create() {
    this->front.z = -1;
    this->world_up.y = 1;
    this->update_rotation();
}

void camera::on_event(SDL_Event &sdl_event) {
    frustum::on_event(sdl_event);
    if (!this->mouse_locked) {
        return;
    }

    switch (sdl_event.type) {
        case SDL_MOUSEMOTION: {
            float delta[2] {
                static_cast<float>(sdl_event.motion.xrel) * this->mouse_sensitivity,
                static_cast<float>(sdl_event.motion.yrel) * this->mouse_sensitivity
            };

            this->rotation.x += delta[0];
            this->rotation.y -= delta[1];

            this->update_rotation();
            break;
        }
    }
}

void camera::update_rotation() {
    glm::vec3 f {};

    float &yaw {this->rotation.x};
    float &pitch {this->rotation.y};

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }

    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    f.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    f.y = glm::sin(glm::radians(pitch));
    f.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

    this->front = glm::normalize(f);
    this->right = glm::normalize(glm::cross(this->front, this->world_up));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}

void camera::set_mouse_locked(bool state) {
    if (this->mouse_locked == state) {
        return;
    }

    switch (this->visible) {
        case enums::state::disable: {
            if (state) {
                SDL_SetRelativeMouseMode(SDL_TRUE);
            } else {
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }

            this->mouse_shown = !state;
            break;
        }

        case enums::state::enable: {
            if (this->mouse_shown) {
                SDL_SetRelativeMouseMode(SDL_FALSE);
                this->mouse_shown = false;
            }
        }

        default:
            break;
    }

    this->mouse_locked = state;
}

bool camera::is_mouse_locked() {
    return this->mouse_locked;
}
