#include "frustum.hpp"

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
    return this->view;
}

void frustum::add_framebuffer(uint32_t framebuffer) {
    this->framebuffer_list.push_back(framebuffer);
}

std::vector<uint32_t> &frustum::get_framebuffer_list() {
    return this->framebuffer_list;
}
