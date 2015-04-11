// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <map>
#include <string>
#include <cstdint>

#include "ft2build.h"
#include FT_FREETYPE_H

namespace Raycer
{
	class RenderTarget;
	class Color;

	class Font
	{
	public:

		Font();
		~Font();

		void load(const std::string& fontFileName, int fontSize);
		void drawText(RenderTarget& renderTarget, size_t x, size_t y, const std::string& text, const Color& color);

		static void initFreeType();
		static void closeFreeType();

	private:

		Font(const Font& font);
		Font& operator=(const Font& font);

		struct Glyph
		{
			uint8_t* alphaMap = nullptr;

			int alphaMapWidth = 0;
			int alphaMapHeight = 0;

			int adjustX = 0;
			int adjustY = 0;
			int advanceX = 0;
		};

		Glyph& getGlyph(char32_t characterCode);

		static FT_Library library;
		FT_Face face = nullptr;
		std::map<char32_t, Glyph> glyphs;
	};
}
