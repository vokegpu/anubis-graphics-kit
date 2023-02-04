#include <SDL2/SDL_events.h>
#include "immshape.hpp"

glm::mat4 immshape::mat4x4_orthographic {};

void immshape::invoke() const {
    glUseProgram(this->p_program->id);
    glDisable(GL_DEPTH_TEST);

    this->p_program->set_uniform_mat4("uOrthographicMatrix", &immshape::mat4x4_orthographic[0][0]);
    this->p_buffer->invoke();
}

void immshape::link(buffering *p_buffer_linked, shading::program *p_program_linked) {
    this->p_buffer = p_buffer_linked;
    this->p_program = p_program_linked;
}

void immshape::revoke() const {
    this->p_buffer->revoke();
    glEnable(GL_DEPTH_TEST);
    glUseProgram(0);
}

void immshape::draw(const glm::vec4 &rectangle, const glm::vec4 &color) const {
    this->p_program->set_uniform_vec4("uRectangle", &rectangle[0]);
    this->p_program->set_uniform_vec4("uColor", &color[0]);
    this->p_buffer->draw();
}