// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <vector>

#include "tinyformat/tinyformat.h"
#include "utf8/utf8.h"

#include "Rendering/Font.h"
#include "Rendering/Framebuffer.h"
#include "Utils/Log.h"
#include "Math/Color.h"

using namespace Raycer;

namespace
{
	FT_Library library = nullptr;
	NamedLog* staticLog = nullptr;
}

Font::Font(BaseLog& baseLog, const std::string& fontFileName, int fontSize)
{
	log = baseLog.getNamedLog("Font");

	if (staticLog == nullptr)
		staticLog = log.get();

	initFreeType();

	log->logInfo("Loading font data from %s (size: %s)", fontFileName, fontSize);

	FT_Error error = FT_New_Face(library, fontFileName.c_str(), 0, &face);

	if (error)
		throw std::runtime_error(tfm::format("Could not load the font: %s", error));

	FT_Set_Pixel_Sizes(face, 0, fontSize);
}

Font::~Font()
{
	for (auto& keyValue : glyphs)
	{
		if (keyValue.second.alphaMap != nullptr)
		{
			_mm_free(keyValue.second.alphaMap);
			keyValue.second.alphaMap = nullptr;
		}
	}
}

void Font::initFreeType()
{
	if (library == nullptr)
	{
		if (staticLog != nullptr)
			staticLog->logInfo("Initializing FreeType");

		FT_Error error = FT_Init_FreeType(&library);

		if (error)
			throw std::runtime_error(tfm::format("Could not initialize the FreeType library: %s", error));
	}
}

void Font::closeFreeType()
{
	if (library != nullptr)
	{
		if (staticLog != nullptr)
			staticLog->logInfo("Closing FreeType");

		FT_Done_FreeType(library);
		library = nullptr;
	}
}

Glyph& Font::getGlyph(char32_t characterCode)
{
	if (glyphs.count(characterCode) == 0)
	{
		int flags = FT_LOAD_RENDER;
		flags |= FT_LOAD_FORCE_AUTOHINT;

		FT_Error error = FT_Load_Char(face, characterCode, flags);

		if (error)
			throw std::runtime_error(tfm::format("Could not load the character (code: %s): %s", characterCode, error));

		FT_Bitmap bitmap = face->glyph->bitmap;
		int bitmapWidth = bitmap.width;
		int bitmapHeight = bitmap.rows;
		int bitmapPitch = bitmap.pitch;

		Glyph glyph;
		glyph.alphaMap = (uint8_t*)_mm_malloc(bitmapWidth * bitmapHeight * sizeof(uint8_t), 16);
		glyph.alphaMapWidth = bitmapWidth;
		glyph.alphaMapHeight = bitmapHeight;

		// http://freetype.org/freetype2/docs/glyphs/glyphs-3.html
		glyph.adjustX = face->glyph->metrics.horiBearingX >> 6;
		glyph.adjustY = (face->glyph->metrics.height - face->glyph->metrics.horiBearingY) >> 6;
		glyph.advanceX = face->glyph->metrics.horiAdvance >> 6;

		uint8_t* bitmapBuffer = bitmap.buffer + ((bitmapHeight - 1) * bitmapPitch);

		// http://freetype.org/freetype2/docs/glyphs/glyphs-7.html
		for (int y = 0; y < bitmapHeight; ++y)
		{
			for (int x = 0; x < bitmapWidth; ++x)
				glyph.alphaMap[y * bitmapWidth + x] = bitmapBuffer[x];

			bitmapBuffer -= bitmapPitch;
		}

		glyphs[characterCode] = glyph;
	}

	return glyphs[characterCode];
}

void Font::drawText(RenderTarget& renderTarget, int x0, int y0, const std::string& text, const Color& color)
{
	std::vector<char32_t> utf32Text;
	utf8::unchecked::utf8to32(text.begin(), text.end(), back_inserter(utf32Text));

	for (char32_t characterCode : utf32Text)
	{
		Glyph& glyph = getGlyph(characterCode);

		int adjustedX = x0 + glyph.adjustX;
		int adjustedY = y0 - glyph.adjustY;

		x0 += glyph.advanceX;

		if (glyph.alphaMapWidth == 0 || glyph.alphaMapHeight == 0)
			continue;

		int width = renderTarget.getWidth();
		int height = renderTarget.getHeight();

		int minX = std::max(adjustedX, 0);
		int maxX = std::min(adjustedX + glyph.alphaMapWidth, width);
		int minY = std::max(adjustedY, 0);
		int maxY = std::min(adjustedY + glyph.alphaMapHeight, height);

		for (int y = minY; y < maxY; ++y)
		{
			for (int x = minX; x < maxX; ++x)
			{
				int alphaMapX = x - adjustedX;
				int alphaMapY = y - adjustedY;
				int alphaMapIndex = alphaMapY * glyph.alphaMapWidth + alphaMapX;
				int glyphAlphaInt = glyph.alphaMap[alphaMapIndex];

				if (glyphAlphaInt == 0)
					continue;

				double glyphAlpha = glyphAlphaInt * (1.0 / 255.0);
				double combinedAlpha = glyphAlpha * color.a;

				renderTarget.setPixel(x, y, Color::alphaBlend(renderTarget.getPixel(x, y), Color(color.r, color.g, color.b, combinedAlpha)));
			}
		}
	}
}
