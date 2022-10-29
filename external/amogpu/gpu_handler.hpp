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
	std::vector<GLuint> concurrent_allocated_textures {};
	std::vector<float> concurrent_allocated_vertices {};
	std::vector<float> concurrent_allocated_texture_coords {};

	uint32_t sizeof_allocated_gpu_data {};
	uint32_t sizeof_previous_allocated_gpu_data {};

	uint32_t sizeof_allocated_vertices {};
	uint32_t sizeof_instanced_allocated_vertices {};

	std::vector<amogpu::gpu_data> allocated_data {};
    std::vector<amogpu::gpu_data> allocated_data_copy {};

	bool should_alloc_new_buffers {};
	bool should_not_create_buffers {};

	GLuint vertex_arr_object {};
	GLuint vbo_vertices {};
	GLuint vbo_texture_coords {};
    bool frustum_depth {};
    float depth {};

	static amogpu::gpu_gl_program fx_shape;
public:
    void set_depth(float depth_test);
    float get_depth();

    void set_frustum_depth(bool depth_test);
    bool get_frustum_depth();

	/*
	 * The current invoked batch.
	 */
	static dynamic_batching* invoked;

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