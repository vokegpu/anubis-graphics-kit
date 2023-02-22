#ifndef AGK_API_UI_IMMEDIATE_SHAPE
#define AGK_API_UI_IMMEDIATE_SHAPE

#include "gpu/tools.hpp"

class immshape {
protected:
    uint32_t bound_texture {};
    bool texture_going_on {};
public:
    static glm::mat4 mat4x4_orthographic;

    buffering *p_buffer {};
    ::asset::shader *p_program {};

    void invoke() const;
    void link(buffering *p_buffer_linked, ::asset::shader *p_program_linked);
    void revoke() const;
    void draw(const glm::vec4 &rectangle, const glm::vec4 &color) const;
};

#endif