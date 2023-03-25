#ifndef AGK_UI_IMMEDIATE_SHAPE
#define AGK_UI_IMMEDIATE_SHAPE

#include "gpu/gpu_buffer.hpp"
#include "asset/shader.hpp"

class shape {
public:
    static glm::mat4 mat4x4ortho;

    buffering *p_buffer {};
    ::asset::shader *p_program {};

    void invoke() const;
    void link(buffering *p_buffer_linked, ::asset::shader *p_program_linked);
    void revoke() const;
    void draw(const glm::vec4 &rectangle, const glm::vec4 &color) const;
};

#endif