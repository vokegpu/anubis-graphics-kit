#ifndef AGK_API_GPU_BUFFERING
#define AGK_API_GPU_BUFFERING

#include "GL/glew.h"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

class buffering {
protected:
    static uint32_t current_type_bind[2];

    std::vector<uint32_t> buffer_list {};
    uint32_t buffer_vao {}, buffer_ebo {};
    uint32_t buffer_list_size {};
public:
    enum class type {
        direct, instanced    
    };

    buffering::type type {};
    uint32_t primitive {GL_TRIANGLES};
    uint64_t stride[3] {};

    void tessellation(int32_t patches);
    void bind(const glm::ivec2 &buffer_type);
    void send(size_t size, void *p_data, uint32_t gl_driver_read_mode);
    void attach(int32_t location, int32_t vec, const glm::ivec2 &array_stride = {0, 0});
    
    void invoke();
    void revoke();
    void draw();
    void free_buffers();
};

#endif