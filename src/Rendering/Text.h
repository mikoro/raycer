// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

#define NOT_USING_FT_GL_NAMESPACE
#include "freetype-gl/freetype-gl.h"
#include "freetype-gl/vertex-buffer.h"
#include "freetype-gl/mat4.h"

namespace Raycer
{
	class Color;

	class Text
	{
	public:

		~Text();

		void initialize(const std::string& fontFileName, double fontSize);
		void setWindowSize(int width, int height);
		void drawText(double x, double y, const Color& color, const std::string& text);
		void render();

	private:

		ftgl::texture_atlas_t* atlas = nullptr;
		ftgl::texture_font_t* font = nullptr;
		ftgl::vertex_buffer_t* buffer = nullptr;

		GLuint programId = 0;
		GLuint samplerId = 0;
		GLuint matrixId = 0;

		ftgl::mat4 mvp;
	};
}
