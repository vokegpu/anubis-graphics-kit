#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "api/util/math.hpp"

glm::mat3 camera3d::get_matrix_camera_view() {
    return glm::lookAt(this->position, this->position + this->front, this->up);
}

void camera3d::on_create() {
    feature::on_create();
    this->front = glm::vec3(0, 0, -1.0f);
    this->up = glm::vec3(0, 0, 0);
    this->world_up = glm::vec3(0, 1.0f, 0);
    this->on_update();
}

void camera3d::on_destroy() {
    feature::on_destroy();
}

void camera3d::on_event(SDL_Event &sdl_event) {
    if (!this->enabled) {
        return;
    }

    switch (sdl_event.type) {
        case SDL_MOUSEMOTION: {
            auto x {static_cast<float>(sdl_event.motion.xrel)}, y {static_cast<float>(sdl_event.motion.yrel)};
            this->yaw += x * this->sensitivity;
            this->pitch = util::clamp(this->pitch - (y * this->sensitivity), -89.0f, 89.0f);
            this->on_update();
            break;
        }
    }
}

void camera3d::on_update() {
    glm::vec3 f {};

    f.x = glm::cos(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));
    f.y = glm::sin(glm::radians(this->pitch));
    f.z = glm::sin(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));

    this->front = glm::normalize(f);
    this->right = glm::normalize(glm::cross(this->front, this->world_up));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}
