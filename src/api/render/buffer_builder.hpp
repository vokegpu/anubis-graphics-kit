#ifndef AGK_API_RENDER_BUFFER_BUILDER_H
#define AGK_API_RENDER_BUFFER_BUILDER_H

#include <GL/glew.h>
#include <iostream>
#include <vector>

enum buffer_builder_mode {
	normal, instanced
};

class buffer_builder {
public:
	std::vector<GLuint> vbo_list {};
	GLuint vao {};
	uint32_t vbo_bound {};
	GLuint primitive {GL_TRIANGLES};
	GLuint vert_amount {};
	buffer_builder_mode mode {};
	GLint instanced_size {};

	void bind();
	void send_data(size_t size, void* data, GLuint draw_mode);
	void shader(GLuint location, GLuint vec_rows, GLuint begin, GLuint end);
	void shader_instanced(GLuint location, GLint vec_rows, GLsizeiptr vec_columns);

	void invoke();
	void revoke();
	void draw();
};

#endif