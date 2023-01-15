#include "frustum.hpp"
#include "api/api.hpp"
#include "api/ui/immshape.hpp"
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 &frustum::get_front() {
    return this->front;
}

glm::vec3 &frustum::get_right() {
    return this->right;
}

glm::vec3 &frustum::get_up() {
    return this->up;
}

glm::vec3 &frustum::get_world_up() {
    return this->world_up;
}

glm::mat4 &frustum::get_view() {
    this->view = glm::lookAt(this->position, this->position + this->front, this->up);
    return this->view;
}

glm::mat4 &frustum::get_perspective() {
    return this->perspective;
}

void frustum::set_fov(float radians_angle) {
    this->fov = radians_angle;
}

float frustum::get_fov() {
    return this->fov;
}

void frustum::on_event(SDL_Event &sdl_event) {
        switch (sdl_event.type) {
        case SDL_WINDOWEVENT: {
            switch (sdl_event.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    api::app.screen_width = sdl_event.window.data1;
                    api::app.screen_height = sdl_event.window.data2;
                    this->process_perspective(sdl_event.window.data1, sdl_event.window.data2);
                    break;
                }                
            }
            
            break;
        }        
    }
}

void frustum::process_perspective(int32_t w, int32_t h) {
    float size[2] {
            static_cast<float>(w),
            static_cast<float>(h)
    };

    this->perspective = glm::perspective(glm::radians(this->fov), size[0] / size[1], 0.1f, agk_perspective_clip_distance);
    immshape::mat4x4_orthographic = glm::ortho(0.0f, size[0], size[1], 0.0f);
}
