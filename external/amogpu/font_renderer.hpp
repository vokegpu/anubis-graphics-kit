#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#ifndef FONT_RENDERER_H
#define FONT_RENDERER_H

#include <iostream>
#include <string>
#include <GL/glew.h>
#include "amogpu/gpu_handler.hpp"

/**
 * The amogpu font renderer, draw strings into a space.
 **/
class font_renderer {
protected:
	static FT_Library ft_library;
	
	FT_Face ft_face;
	FT_GlyphSlot ft_glyph_slot;
	FT_Bool use_kerneking;
	FT_UInt previous;
	FT_Vector_ previous_char_vec;

	uint32_t texture_width;
	uint32_t texture_height;

	std::string current_font_path;
	uint8_t current_font_size;

	dynamic_batching* binded_batch;
	uint8_t batch_mode = 0;

	amogpu::font_char allocated_font_char[256];
public:
	/*
	 * The texture bitmap.
	 */
	GLuint texture_bitmap;

	/*
	 * Get the current binded batch.
	 */
	dynamic_batching* batch();

	/*
	 * Init Freetype library.
	 */
	static void init();

	/*
	 * End the Freetype library.
	 */
	static void end_ft_library();

	/*
	 * Load ttf font and size.
	 */
	void load(const std::string &font_path, uint8_t font_size);

	/*
	 * Specify what batch the font renderer will use.
	 */
	void from(dynamic_batching *concurrent_batch);

	/*
	 * Specify what batch the font renderer will use.
	 */
	void from(uint8_t mode);

	/*
	 * Get input text width.
	 */
	float get_text_width(const std::string &text);

	/*
	 * Get current font height.
	 */
	float get_text_height();

	/*
	 * Send data for GPU to display text into screen space.
	 */
	void render(const std::string &text, float x, float y, const amogpu::vec4f &color);
};

#endif