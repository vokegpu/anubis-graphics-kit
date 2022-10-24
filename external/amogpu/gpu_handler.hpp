#pragma once
#ifndef GPU_HANDLER_H
#define GPU_HANDLER_H

#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "amogpu/core.hpp"

/**
 * Batch but dynamic, high performance.
 **/
struct dynamic_batching {
protected:
	std::vector<float> concurrent_allocated_textures;
	std::vector<float> concurrent_allocated_vertices;
	std::vector<float> concurrent_allocated_texture_coords;

	uint32_t sizeof_allocated_gpu_data;
	uint32_t sizeof_previous_allocated_gpu_data;

	uint32_t sizeof_allocated_vertices;
	uint32_t sizeof_instanced_allocated_vertices;

	amogpu::gpu_data allocated_gpu_data[2048];
	bool should_alloc_new_buffers;
	bool should_not_create_buffers;

	GLuint vertex_arr_object;
	GLuint vbo_vertices;
	GLuint vbo_texture_coords;

	static amogpu::gpu_gl_program fx_shape;
public:
	/*
	 * The current invoked batch.
	 */
	static dynamic_batching* invoked;

	/*
	 * The current z depth position of all draws.
	 */
	static uint32_t depth;

	/*
	 * Init the dynamic batching.
	 */
	static void init();

	/*
	 * Update global matrices.
	 */
	static void matrix();

	/*
	 * Invoke GPU.
	 */
	void invoke();

	/*
	 * Create a sub-section into GPU section.
	 */
	void instance(float x, float y, int32_t factor = -1);

	/*
	 * For a simple shapes.
	 */
	void modal(float w, float h);

	/*
	 * For complex shapes.
	 */
	void factor(int32_t factor);

	/*
	 * Fill with RGBA color normalised.
	 */
	void fill(const amogpu::vec4f &color);

    /*
	 * Fill with RGBA color normalised.
	 */
	void fill(float r, float g, float b, float a = 1.0f);

    /*
	 * Add one vertex.
	 */
	void vertex(float x, float y);

    /*
	 * Bind a texture.
	 */
	void bind(GLuint texture);

	/*
	 * Add one uv coordinates for texture, if there is not any binded texture just put 0.0f at uv(s) parameters.
	 */
	void coords(float u, float v);

	/*
	 * End the sub-segment.
	 */
	void next();

	/*
	 * End GPU communication.
	 */
	void revoke();

	/*
	 * Draw the current batch.
	 */
	void draw();

	/*
	 * Delete the buffers of batch.
	 */
	void free_buffers();
};

#endif