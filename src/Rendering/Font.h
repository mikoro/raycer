// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <map>
#include <memory>
#include <string>

#include "ft2build.h"
#include FT_FREETYPE_H

namespace Raycer
{
	class BaseLog;
	class NamedLog;
	class RenderTarget;
	class Color;

	struct Glyph
	{
		uint8_t* alphaMap = nullptr;

		int alphaMapWidth = 0;
		int alphaMapHeight = 0;

		int adjustX = 0;
		int adjustY = 0;
		int advanceX = 0;
	};

	class Font
	{
	public:

		Font(BaseLog& baseLog, const std::string& fontFileName, int fontSize);
		~Font();

		static void initFreeType();
		static void closeFreeType();

		void drawText(RenderTarget& renderTarget, int x, int y, const std::string& text, const Color& color);

	private:

		Glyph& getGlyph(char32_t characterCode);

		std::unique_ptr<NamedLog> log;

		FT_Face face = nullptr;
		std::map<char32_t, Glyph> glyphs;
	};
}
