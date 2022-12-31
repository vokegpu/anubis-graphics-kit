#ifndef AGK_API_GPU_BUFFERING
#define AGK_API_GPU_BUFFERING

#include "GL/glew.h"
#include <iostream>
#include <vector>
#include "api/feature/feature.hpp"

enum bufferingmode {
	normal, instanced
};

class buffering : public feature {
public:
    const char* tag {};
    int32_t id {};

	uint32_t buffer_vao {0};
    uint32_t buffer_ebo {};

    uint32_t primitive {GL_TRIANGLES};
    int32_t stride_begin {};
    int32_t stride_end {};
    int32_t instanced_size {};

    std::vector<GLuint> buffer_list {};
    uint32_t buffer_list_size {};

    bufferingmode mode {};

	void bind();
    void send_data(GLint size, void* data, GLuint draw_mode);
	void shader(GLuint location, GLint vec_rows, GLint begin, GLsizeiptr end);
	void shader_instanced(GLuint location, GLint vec_rows, GLsizeiptr vec_columns, GLsizeiptr size);

    void bind_ebo();
    void send_indexing_data(GLint size, void* data, GLuint draw_mode);

	void invoke();
	void revoke();

    void destroy();
    void draw();
};

#endif