#pragma once
#ifndef SHAPE_BUILDER_H
#define SHAPE_BUILDER_H

#include <GL/glew.h>
#include "core.hpp"

/**
 * Buiild static geometry shape, fast and immediate.
 **/
class shape_builder {
protected:
	static amogpu::gpu_gl_program fx_shape;
	
	static GLuint vertex_array;
	static GLuint vertex_buffer;

	amogpu::shape enum_flag_format;
	amogpu::gpu_data concurrent_gpu_data;
public:
    float depth {};

	/*
	 * Init shape builder.
	 */
	static void init();

	/*
	 * Delete buffers.
	 */
	static void free_buffers();

	/*
	 * Invoke GPU data.
	 */
	void invoke();

	/*
	 * Revoke GPU data.
	 */
	void revoke();

	/*
	 * Build a shape.
	 */
	void build(const amogpu::shape &format, const amogpu::vec4f &color, GLuint texture = 0);

	/*
	 * Set texture uv.
	 */
	void modal(float x, float y, float w, float h);

	/*
	 * Draw the shape.
	 */
	void draw(float x, float y, float w, float h);
};

#endif