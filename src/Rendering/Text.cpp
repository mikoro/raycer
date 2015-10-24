// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Text.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/GLHelper.h"
#include "Math/Color.h"

using namespace Raycer;

namespace
{
	typedef struct
	{
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
	font = ftgl::texture_font_new_from_file(atlas, float(fontSize), fontFileName.c_str());

	if (font == nullptr)
		throw std::runtime_error("Could not load font");

	buffer = ftgl::vertex_buffer_new("vertex:3f,tex_coord:2f,color:4f");

	programId = GLHelper::buildProgram("data/shaders/text.vert", "data/shaders/text.frag");
	samplerId = glGetUniformLocation(programId, "tex0");
	matrixId = glGetUniformLocation(programId, "mvp");

	ftgl::mat4_set_identity(&mvp);
}

void Text::setWindowSize(uint64_t width, uint64_t height)
{
	ftgl::mat4_set_orthographic(&mvp, 0, float(width), 0, float(height), -1, 1);
}

void Text::drawText(double x, double y, const Color& color, const std::string& text)
{
	float tx = float(x);
	float ty = float(y);

	std::vector<wchar_t> utf16Text;
	utf8::unchecked::utf8to16(text.begin(), text.end(), back_inserter(utf16Text));

	for (uint64_t i = 0; i < utf16Text.size(); ++i)
	{
		ftgl::texture_glyph_t* glyph = ftgl::texture_font_get_glyph(font, utf16Text[i]);

		if (glyph != nullptr)
		{
			float kerning = 0.0f;

			if (i > 0)
				kerning = ftgl::texture_glyph_get_kerning(glyph, utf16Text[i - 1]);

			tx += kerning;

			float x0 = tx + glyph->offset_x;
			float y0 = ty + glyph->offset_y;
			float x1 = x0 + glyph->width;
			float y1 = y0 - glyph->height;

			float s0 = glyph->s0;
			float t0 = glyph->t0;
			float s1 = glyph->s1;
			float t1 = glyph->t1;

			GLuint indices[6] = { 0, 1, 2, 0, 2, 3 };

			vertex_t vertices[4] = {
				{ x0, y0, 0, s0, t0, float(color.r), float(color.g), float(color.b), float(color.a) },
				{ x0, y1, 0, s0, t1, float(color.r), float(color.g), float(color.b), float(color.a) },
				{ x1, y1, 0, s1, t1, float(color.r), float(color.g), float(color.b), float(color.a) },
				{ x1, y0, 0, s1, t0, float(color.r), float(color.g), float(color.b), float(color.a) } };

			ftgl::vertex_buffer_push_back(buffer, vertices, 4, indices, 6);
			tx += glyph->advance_x;
		}
	}
}

void Text::render()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(programId);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(samplerId, 0);
	glUniformMatrix4fv(matrixId, 1, 0, mvp.data);

	glBindTexture(GL_TEXTURE_2D, atlas->id);

	ftgl::vertex_buffer_render(buffer, GL_TRIANGLES);
	ftgl::vertex_buffer_clear(buffer);

	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
	glDisable(GL_BLEND);

	GLHelper::checkError("Could not render text");
}
