#include <SDL2/SDL_events.h>
#include "immshape.hpp"

glm::mat4 immshape::mat4x4_orthographic {};

void immshape::invoke() {
    glUseProgram(this->p_program->id);
    this->p_program->set_uniform_mat4("MatrixPerspective", &immshape::mat4x4_orthographic[0][0]);
    this->p_buffer->invoke();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void immshape::link(buffering *p_buffer_linked, shading::program *p_program_linked) {
    this->p_buffer = p_buffer_linked;
    this->p_program = p_program_linked;
}

void immshape::revoke() {
    this->p_buffer->revoke();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glUseProgram(0);
}

void immshape::bind_texture(uint32_t texture) {
    this->texture_going_on = true;
    this->p_program->set_uniform_int("ActiveTexture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void immshape::draw(const glm::vec4 &rectangle, const glm::vec4 &color) {
    this->p_program->set_uniform_bool("TextureEnabled", this->texture_going_on);
    this->p_program->set_uniform_vec4("Rectangle", &rectangle[0]);
    this->p_program->set_uniform_vec4("Color", &color[0]);
    this->p_buffer->draw();

    if (this->texture_going_on) {
        glBindTexture(GL_TEXTURE_2D, 0);
        this->texture_going_on = false;
    }
}