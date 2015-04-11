// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <vector>

#include <GL/glew.h>

#define NOT_USING_FT_GL_NAMESPACE
#include "freetype-gl/freetype-gl.h"
#include "freetype-gl/shader.h"
#include "freetype-gl/vertex-attribute.h"

#include "utf8/utf8.h"
#include "tinyformat/tinyformat.h"

#include "Rendering/Text.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Errors.h"
#include "Math/Color.h"

using namespace Raycer;

namespace
{
	typedef struct {
		float x, y, z;
		float s, t;
		float r, g, b, a;
	} vertex_t;
}

Text::~Text()
{
	if (atlas != nullptr)
	{
		ftgl::texture_atlas_delete(atlas);
		atlas = nullptr;
	}

	if (font != nullptr)
	{
		ftgl::texture_font_delete(font);
		font = nullptr;
	}

	if (buffer != nullptr)
	{
		ftgl::vertex_buffer_delete(buffer);
		buffer = nullptr;
	}
}

void Text::initialize(const std::string& fontFileName, double fontSize)
{
	App::getLog().logInfo("Loading font from %s", fontFileName);

	atlas = ftgl::texture_atlas_new(512, 512, 1);
	font = ftgl::texture_font_new_from_file(atlas, (float)fontSize, fontFileName.c_str());

	if (font == nullptr)
		throw std::runtime_error("Could not load font");

	buffer = ftgl::vertex_buffer_new("vertex:3f,tex_coord:2f,color:4f");
	ftgl::texture_font_load_glyphs(font, L" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
	shader = ftgl::shader_load("data/shaders/text.vert", "data/shaders/text.frag");

	ftgl::mat4_set_identity(&model);
	ftgl::mat4_set_identity(&view);
	ftgl::mat4_set_identity(&projection);
}

void Text::setWindowSize(size_t width, size_t height)
{
	ftgl::mat4_set_orthographic(&projection, 0, (float)width, 0, (float)height, -1, 1);
}

void Text::drawText(double x, double y, const Color& color, const std::string& text)
{
	float tx = (float)x;
	float ty = (float)y;

	std::vector<wchar_t> utf16Text;
	utf8::unchecked::utf8to16(text.begin(), text.end(), back_inserter(utf16Text));

	for (size_t i = 0; i < utf16Text.size(); ++i)
	{
		ftgl::texture_glyph_t* glyph = ftgl::texture_font_get_glyph(font, utf16Text[i]);

		if (glyph != nullptr)
		{
			float kerning = 0.0f;

			if (i > 0)
				kerning = ftgl::texture_glyph_get_kerning(glyph, utf16Text[i - 1]);

			tx += kerning;

			float x0 = tx + (float)glyph->offset_x;
			float y0 = ty + (float)glyph->offset_y;
			float x1 = x0 + (float)glyph->width;
			float y1 = y0 - (float)glyph->height;

			float s0 = glyph->s0;
			float t0 = glyph->t0;
			float s1 = glyph->s1;
			float t1 = glyph->t1;

			GLuint indices[6] = { 0, 1, 2, 0, 2, 3 };

			vertex_t vertices[4] = {
				{ x0, y0, 0, s0, t0, (float)color.r, (float)color.g, (float)color.b, (float)color.a },
				{ x0, y1, 0, s0, t1, (float)color.r, (float)color.g, (float)color.b, (float)color.a },
				{ x1, y1, 0, s1, t1, (float)color.r, (float)color.g, (float)color.b, (float)color.a },
				{ x1, y0, 0, s1, t0, (float)color.r, (float)color.g, (float)color.b, (float)color.a } };

			ftgl::vertex_buffer_push_back(buffer, vertices, 4, indices, 6);
			tx += glyph->advance_x;
		}
	}
}

void Text::render()
{
	glBindTexture(GL_TEXTURE_2D, atlas->id);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(shader);
	glUniform1i(glGetUniformLocation(shader, "texture"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, 0, model.data);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, 0, view.data);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, 0, projection.data);

	ftgl::vertex_buffer_render(buffer, GL_TRIANGLES);
	ftgl::vertex_buffer_clear(buffer);

	glUseProgram(0);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	checkGlError("Could not render text");
}
