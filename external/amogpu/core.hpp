#pragma once
#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <string>

/**
 * @author Rina
 * @since 28/07/22 at 15:07pm
 * 
 * THE AMOGPU LIBRARY.
 * ALL CREDITS RESERVED TO RINA (aka MrsRina).
 * FREE COMERCIAL USE, YOU NEED USE THE PUBLIC LICENSE.
 **/
namespace amogpu {
	extern uint8_t dynamic;
	extern uint8_t invoked;

	extern float matrix_viewport[4];
	extern float matrix_projection_ortho[16];
	extern std::string gl_version;

	/*
	 * Shape type to shpae builder.
	 */
	enum shape {
		RECT, CIRCLE
	};

	/*
	 * Store font data.
	 */
	struct font_char {
		float x = 0;
		float texture_x = 0;
	
		float w = 0;
		float h = 0;
	
		float top = 0;
		float left = 0;
	};
	
	/**
	 * Mini-programs on GPU.
	 **/
	struct gpu_gl_program {
		GLuint program = 0;
		bool validation = false;
	
		void use();
		void end();
	
		/* Start of uniform setters. */
		void setb(const std::string &uniform_name, bool val);
		void seti(const std::string &uniform_name, int32_t val);
		void setf(const std::string &uniform_name, float val);
		void set2f(const std::string &uniform_name, const float* val);
		void set4f(const std::string &uniform_name, const float* val);
		void setm4f(const std::string &uniform_name, const float* val);
		void set3f(const std::string &uniform_name, const float* val);
		void setm3f(const std::string &uniform_name, const float* val);
		/* End of uniform setters. */
	};
	
	/**
	 * Store GPU data.
	 **/
	struct gpu_data {
		uint32_t factor;
		float color[4];
		float rect[4];
	
		GLint begin = 0;
		GLint end = 0;
	
		uint8_t texture_slot = 0;
		GLuint texture = 0;
	};

	/*
	 * Rectangle.
	 */
	struct rect {
		float x = 0.0f;
		float y = 0.0f;
		float w = 0.0f;
		float h = 0.0f;

		/*
		 * Detect if the rect is colliding with a point in space.
		 */
		bool aabb_collide_with_point(float x, float y);

		void operator += (amogpu::rect &rect) {
			this->x += rect.x;
			this->y += rect.y;
		}

		void operator -= (amogpu::rect &rect) {
			this->x -= rect.x;
			this->y -= rect.y;
		}
	};

	/*
	 * Vector.
	 */
	struct vec4f {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;

		vec4f(float x, float y, float z, float w);
	};

	/*
	 * DEPRECATED
	 * Global settings of a clock.
	 */
	struct clock {
		static float dt;
		static uint32_t fps;
	};

	/*
	 * Init amogpu library.
	 */ 
	void init();

	/*
	 * Quit amogpu library.
	 */ 
	void quit();

	/*
	 * Update matrices.
	 */
	void matrix();

	/*
	 * Send output log.
	 */
	void log(const std::string &input_str);

	/*
	 * Open and get string data output from file.
	 */
	bool read_file(std::string &input_str, const std::string &path);
	
	/*
	 * Pass to matrix the current window viewport.
	 */
	void viewport(float* mat);

	/*
	 * Pass to matrix the projection view ortho 2D.
	 */
	void projection_view_ortho(float* mat, float left, float right, float bottom, float top);

	/*
	 * Compile shader.
	 */
	bool compile_shader(GLuint &shader, GLuint mode, const char* src);

	/*
     * Create mini-program into GPU.
	 */
	bool create_program(gpu_gl_program &program, const char* vsh_path, const char* fsh_path);

	/*
     * Create mini-program into GPU without load a file.
	 */
	bool create_program_from_src(gpu_gl_program &program, const char* vsh_src, const char* fsh_src);
}


#endif