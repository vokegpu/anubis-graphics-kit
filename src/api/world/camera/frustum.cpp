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
    const float half_v_side {this->far * glm::tan(this->fov * 0.5f)};
    const float half_h_side {half_v_side * this->aspect};
    const glm::vec3 front_mult_far {this->far * this->front};
    
    /* The distance is perpendicular to P. */
    this->near_face = {this->position + this->near * this->front, this->front};
    this->far_face = {this->position + front_mult_far, -this->front};
    this->right_face =  {this->position, glm::cross(front_mult_far - this->right * half_h_side, this->up)};
    this->left_face = {this->position, glm::cross(this->position, glm::cross(this->up, front_mult_far + this->right * half_h_side))};
    this->top_face = {this->position, glm::cross(this->right, front_mult_far - this->up * half_v_side)};
    this->bottom_face = {this->position, glm::cross(front_mult_far + this->up * half_v_side, this->right)};

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
    const float size[2] {
            static_cast<float>(w),
            static_cast<float>(h)
    };

    this->aspect = size[0] / size[1];
    this->perspective = glm::perspective(glm::radians(this->fov), this->aspect, 0.1f, agk_perspective_clip_distance);
    immshape::mat4x4_orthographic = glm::ortho(0.0f, size[0], size[1], 0.0f);

    api::app.screen_width = w;
    api::app.screen_height = h;

    this->far = agk_perspective_clip_distance;
    this->near = 0.1f;
    this->w = size[0];
    this->h = size[1];

    std::string msg {"Window resized ("};
    msg += std::to_string(w);
    msg += ", ";
    msg += std::to_string(h);
    msg += ')';

    util::log(msg);
}

bool frustum::viewing(const glm::mat4 &mat4x4_model, const glm::vec3 &scale, const util::aabb &axis_aligned_bounding_box) {
    // @TODO Add frustum view culling.
    return true;

    const glm::vec3 global_center {mat4x4_model * glm::vec4(axis_aligned_bounding_box.center, 1.0f)};

    const glm::vec3 r {mat4x4_model[0] * axis_aligned_bounding_box.extents.x};
    const glm::vec3 u {mat4x4_model[1] * axis_aligned_bounding_box.extents.y};
    const glm::vec3 f {-mat4x4_model[2] * axis_aligned_bounding_box.extents.z};

    const float ii {
        glm::abs(glm::dot({1.0f, 0.0f, 0.0f}, r)) +
        glm::abs(glm::dot({1.0f, 0.0f, 0.0f}, u)) +
        glm::abs(glm::dot({1.0f, 0.0f, 0.0f}, f))
    };

    const float ij {
        glm::abs(glm::dot({0.0f, 1.0f, 0.0f}, r)) +
        glm::abs(glm::dot({0.0f, 1.0f, 0.0f}, u)) +
        glm::abs(glm::dot({0.0f, 1.0f, 0.0f}, f))
    };

    const float ik {
        glm::abs(glm::dot({0.0f, 0.0f, 1.0f}, r)) +
        glm::abs(glm::dot({0.0f, 0.0f, 1.0f}, u)) +
        glm::abs(glm::dot({0.0f, 0.0f, 1.0f}, f))
    };

    util::aabb global_aabb {};
    global_aabb.center = global_center;
    global_aabb.extents = {ii, ij, ik};

    return (
        util::check_is_on_forward_plane(global_aabb, this->left_face) &&
        util::check_is_on_forward_plane(global_aabb, this->right_face) &&
        util::check_is_on_forward_plane(global_aabb, this->top_face) &&
        util::check_is_on_forward_plane(global_aabb, this->bottom_face) &&
        util::check_is_on_forward_plane(global_aabb, this->near_face) &&
        util::check_is_on_forward_plane(global_aabb, this->far_face)
    );
}