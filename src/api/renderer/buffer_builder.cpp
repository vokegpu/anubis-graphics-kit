#include "buffer_builder.hpp"
#include "api/util/env.hpp"

void buffer_builder::bind() {
	GLuint vbo {};

	if (this->vbo_list.size() >= this->vbo_bound) {
		glGenBuffers(1, &vbo);
		this->vbo_list.push_back(vbo);
	} else {
		vbo = this->vbo_list.at(this->vbo_bound);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	this->vbo_bound++;
}

void buffer_builder::send_data(GLint size, void* data, GLuint draw_mode) {
	glBufferData(GL_ARRAY_BUFFER, size, data, draw_mode);
}

void buffer_builder::shader(GLuint location, GLint vec_rows, GLint begin, GLsizeiptr end) {
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, vec_rows, GL_FLOAT, GL_FALSE, begin, (void*) end);
}

void buffer_builder::shader_instanced(GLuint location, GLint vec_rows, GLsizeiptr vec_columns) {
	GLint vec_area {static_cast<GLint>(vec_rows * vec_columns)};
	GLsizeiptr stride_size {};

	for (GLuint shader_location {location}; shader_location < location + vec_columns; shader_location++) {
		glEnableVertexAttribArray(shader_location);
		glVertexAttribPointer(shader_location, vec_rows, GL_FLOAT, GL_FALSE, vec_area, (void*) stride_size);
		glVertexAttribDivisor(shader_location, 1);
		stride_size += vec_rows;
	}
}

void buffer_builder::invoke() {
	this->vbo_bound = 0;
	if (this->vao == 0) glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);
}

void buffer_builder::revoke() {
	if (this->vbo_bound < this->vbo_list.size()) {
		const std::vector<GLuint> cache {this->vbo_list};
		for (size_t it {this->vbo_bound}; it < cache.size(); it++) {
			glDeleteBuffers(1, &cache.at(it));
			this->vbo_list.erase(this->vbo_list.begin() + it);
		}
	}

	glBindVertexArray(0);
}

void buffer_builder::on_create() {
    feature::on_create();
}

void buffer_builder::on_destroy() {
    feature::on_destroy();
    glDeleteVertexArrays(1, &this->vao);

    for (GLuint &vbos : this->vbo_list) {
        glDeleteBuffers(1, &vbos);
    }

    this->vbo_list.clear();
}

void buffer_builder::on_render() {
    glBindVertexArray(this->vao);

    switch (this->mode) {
        case buffer_builder_mode::normal: {
            glDrawArrays(this->primitive, 0, this->vert_amount);
            break;
        }

        case buffer_builder_mode::instanced: {
            glDrawArraysInstanced(this->primitive, 0, this->vert_amount, this->instanced_size);
            break;
        }
    }

    glBindVertexArray(0);
}

buffer_builder::buffer_builder(const char* str_tag) {
    this->tag = str_tag;
}
