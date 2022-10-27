#ifndef AGK_API_GPU_BUFFER_BUILDER_H
#define AGK_API_GPU_BUFFER_BUILDER_H

#include <GL/glew.h>
#include <iostream>
#include <vector>
#include "api/feature/feature.hpp"

enum buffer_builder_mode {
	normal, instanced
};

class buffer_builder : public feature {
public:
    const char* tag {};
    uint32_t id {};

	GLuint vao {0};
    GLuint ebo {};
    uint32_t vbo_bound {};
    GLuint primitive {GL_TRIANGLES};
    GLint vert_amount {};
    GLint instanced_size {};

    std::vector<GLuint> vbo_list {};
    buffer_builder_mode mode {};
    buffer_builder(const char*);

	void bind();
	void send_data(GLint size, void* data, GLuint draw_mode);
	void shader(GLuint location, GLint vec_rows, GLint begin, GLsizeiptr end);
	void shader_instanced(GLuint location, GLint vec_rows, GLsizeiptr vec_columns);
    void bind_ebo();
    void send_indexing_data(GLint size, void* data, GLuint draw_mode);

	void invoke();
	void revoke();

    void on_create() override;
    void on_destroy() override;
    void on_render() override;
};

#endif