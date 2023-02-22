#include <SDL2/SDL_events.h>
#include "shape.hpp"

glm::mat4 shape::mat4x4_orthographic {};

void shape::invoke() const {
    this->p_program->invoke();
    glDisable(GL_DEPTH_TEST);

    this->p_program->set_uniform_mat4("uOrthographicMatrix", &shape::mat4x4_orthographic[0][0]);
    this->p_buffer->invoke();
}

void shape::link(buffering *p_buffer_linked, ::asset::shader *p_program_linked) {
    this->p_buffer = p_buffer_linked;
    this->p_program = p_program_linked;
}

void shape::revoke() const {
    this->p_buffer->revoke();
    glEnable(GL_DEPTH_TEST);
    glUseProgram(0);
}

void shape::draw(const glm::vec4 &rectangle, const glm::vec4 &color) const {
    this->p_program->set_uniform_vec4("uRectangle", &rectangle[0]);
    this->p_program->set_uniform_vec4("uColor", &color[0]);
    this->p_buffer->draw();
}