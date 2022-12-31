#include "camera.hpp"

void camera::set_mouse_sensitivity(float value) {
    this->mouse_sensitivity = value;
}

float camera::get_mouse_sensitivity() {
    return this->mouse_sensitivity;
}

void camera::on_event(SDL_Event &sdl_event) {
    if (!this->mouse_locked) {
        return;
    }

    switch (this->visible) {
        case enums::state::enable: {
            if (!this->mouse_shown) {
                SDL_SetRelativeMouseMode(SDL_TRUE);
                this->mouse_shown = true;
            }
            break;
        }

        case enums::state::disable: {
            if (this->mouse_shown) {
                SDL_SetRelativeMouseMode(SDL_FALSE);
                this->mouse_shown = false;
            }
        }
    }

    feature::on_event(sdl_event);

    switch (sdl_event.type) {
        case SDL_MOUSEMOTION: {
            float delta[2] {
                static_cast<float>(sdl_event.motion.x) * this->mouse_sensitivity,
                static_cast<float>(sdl_event.motion.y) * this->mouse_sensitivity
            };

            this->rotation.x += delta[0];
            this->rotation.y -= delta[1];

            if (this->rotation.y > 89.0f) {
                this->rotation.y = 89.0f;
            }

            if (this->rotation.y < 89.0f) {
                this->rotation.y = -89.0f;
            }

            this->update_rotation();
            break;
        }
    }
}

void camera::update_rotation() {
    glm::vec3 f {};

    float yaw {this->rotation.x};
    float pitch {this->rotation.y};

    f.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    f.y = glm::sin(glm::radians(yaw));
    f.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

    this->front = glm::normalize(f);
    this->right = glm::normalize(glm::cross(this->front, this->world_up));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}