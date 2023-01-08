#ifndef AGK_API_GPU_BUFFERING
#define AGK_API_GPU_BUFFERING

#include "GL/glew.h"
#include <iostream>
#include <vector>
#include "api/feature/feature.hpp"
#include <glm/glm.hpp>

class buffering : public feature {
protected:
    static uint32_t current_type_bind[2];

    std::vector<uint32_t> buffer_list {};
    uint32_t buffer_vao {}, buffer_ebo {};
    uint32_t buffer_list_size {};
    uint32_t shader_location_index {};
public:
    enum class type {
        direct, instanced    
    };

    buffering::type type {};
    uint32_t primitive {GL_TRIANGLES};
    int32_t stride[3] {};

    void bind(const glm::ivec2 &buffer_type);
    void send(size_t size, void *p_data, uint32_t gl_driver_read_mode);
    void attach(int32_t location, int32_t vec, const glm::ivec2 &array_stride = {0, 0});
    
    void invoke();
    void revoke();
    void draw();
};

#endif