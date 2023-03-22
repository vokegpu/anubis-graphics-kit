#include "frustum.hpp"
#include "agk.hpp"
#include "ui/shape.hpp"
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
    this->view = glm::lookAt(this->transform.position, this->transform.position + this->front, this->up);
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
                    agk::app.screen_width = sdl_event.window.data1;
                    agk::app.screen_height = sdl_event.window.data2;
                    this->process_perspective(sdl_event.window.data1, sdl_event.window.data2);
                    break;
                }                
            }
            
            break;
        }        
    }
}

void frustum::process_perspective(int32_t width, int32_t height) {
    const float size[2] {
            static_cast<float>(width),
            static_cast<float>(height)
    };

    this->aspect = size[0] / size[1];
    this->far = agk_perspective_clip_distance;
    this->near = 0.03f;

    this->perspective = glm::perspective(glm::radians(this->fov), this->aspect, this->near, this->far);
    shape::mat4x4_orthographic = glm::ortho(0.0f, size[0], size[1], 0.0f);

    if (this->w != size[0] || this->h != size[1]) {
        std::string msg{"Window resized ("};
        msg += std::to_string(width);
        msg += ", ";
        msg += std::to_string(height);
        msg += ')';

        util::log(msg);
    }

    agk::app.screen_width = width;
    agk::app.screen_height = height;
    this->w = size[0];
    this->h = size[1];
}

bool frustum::viewing(glm::mat4 &mat4x4_model, util::aabb &aabb) {
    const glm::vec3 min {glm::vec3(mat4x4_model * glm::vec4(aabb.min, 1.0f))};
    const glm::vec3 max {glm::vec3(mat4x4_model * glm::vec4(aabb.max, 1.0f))};
    glm::vec3 p {};

    for (uint8_t it {}; it < 6; it++) {
        p.x = this->planes[it].n.x > 0.0f ? max.x : min.x;
        p.y = this->planes[it].n.y > 0.0f ? max.y : min.y;
        p.z = this->planes[it].n.z > 0.0f ? max.z : min.z;

        if (glm::dot(this->planes[it].n, p) + this->planes[it].distance < -1.666) {
            return false;
        }
    }

    return true;
}

glm::mat4 &frustum::get_mvp() {
    this->mvp = this->perspective * this->view;

    float m0 {this->mvp[0][0]};
    float m1 {this->mvp[0][1]};
    float m2 {this->mvp[0][2]};
    float m3 {this->mvp[0][3]};

    float m4 {this->mvp[1][0]};
    float m5 {this->mvp[1][1]};
    float m6 {this->mvp[1][2]};
    float m7 {this->mvp[1][3]};

    float m8 {this->mvp[2][0]};
    float m9 {this->mvp[2][1]};
    float m10 {this->mvp[2][2]};
    float m11 {this->mvp[2][3]};

    float m12 {this->mvp[3][0]};
    float m13 {this->mvp[3][1]};
    float m14 {this->mvp[3][2]};
    float m15 {this->mvp[3][3]};

    this->planes[0].set_plane(glm::vec3(m3 - m0, m7 - m4, m11 - m8), m15 - m12);
    this->planes[1].set_plane(glm::vec3(m3 + m0, m7 + m4, m11 + m8), m15 + m12);
    this->planes[2].set_plane(glm::vec3(m3 + m1, m7 + m5, m11 + m9), m15 + m13);
    this->planes[3].set_plane(glm::vec3(m3 - m1, m7 - m5, m11 - m9), m15 - m13);
    this->planes[4].set_plane(glm::vec3(m3 - m2, m7 - m6, m11 - m10), m15 - m14);
    this->planes[5].set_plane(glm::vec3(m3 + m2, m7 + m6, m11 + m10), m15 + m14);
    return this->mvp;
}

void viewplane::set_plane(const glm::vec3 &p, float dist) {
    float len {glm::length(p)};
    if (len > 0.0f) {
        len = 1.0f / len;
    }

    this->n = p * len;
    this->distance = dist * len;
}
