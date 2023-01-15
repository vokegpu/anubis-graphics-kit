#ifndef AGK_API_UI_IMMEDIATE_SHAPE
#define AGK_API_UI_IMMEDIATE_SHAPE

#include "api/gpu/buffering.hpp"

class immshape {
protected:
    buffering *p_buffer {};
    shading::program *p_program {};

    uint32_t bound_texture {};
    bool texture_going_on {};
public:
    static glm::mat4 mat4x4_orthographic;

    void invoke();
    void link(buffering *p_buffer_linked, shading::program *p_program_linked);
    void revoke();

    void bind_texture(uint32_t texture);
    void draw(const glm::vec4 &rectangle, const glm::vec4 &color);
};

#endif