#include "agk_camera.hpp"

agk_camera::agk_camera() {
    this->position = glm::vec3(0.0f, 0.0f, 3.0f);
    this->up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->fov = 60.0f;

    this->on_update();
}

agk_camera::~agk_camera() {
}

void agk_camera::on_event(SDL_Event &sdl_event) {
    agk_feature::on_event(sdl_event);

    switch (sdl_event.type) {
        case SDL_MOUSEMOTION: {
            if (this->mouse_locked) {
                float mouse_delta_x = (static_cast<float>(sdl_event.motion.x) - this->previous_mx) * this->mouse_sensitivity;
                float mouse_delta_y = (static_cast<float>(sdl_event.motion.y) - this->previous_my) * this->mouse_sensitivity;

                this->yaw += mouse_delta_x;
                this->pitch += mouse_delta_y;

                if (this->pitch > 89.0f) {
                    this->pitch = 89.0f;
                }

                if (this->pitch < -89.0f) {
                    this->pitch = -89.0f;
                }

                this->on_update();
            }

            break;
        }
    }
}

void agk_camera::on_update() {
    agk_feature::on_update();

    glm::vec3 vec_front;

    vec_front.x = glm::cos(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));
    vec_front.y = glm::sin(glm::radians(this->pitch));
    vec_front.z = glm::sin(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));

    this->front = glm::normalize(vec_front);
    this->right = glm::normalize(glm::cross(this->front, this->world_up));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}

void agk_camera::on_render() {
    agk_feature::on_render();
}
