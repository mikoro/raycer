// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <cstdint>

namespace Raycer
{
	class Color
	{
	public:

		Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
		explicit Color(int r, int g, int b, int a = 255);
		Color(uint32_t abgr);

		uint32_t getAbgrValue() const;
		bool isTransparent() const;
		void clamp();
		Color clamped() const;

		static Color lerp(const Color& start, const Color& end, float alpha);
		static Color alphaBlend(const Color& first, const Color& second);
		static uint32_t alphaBlend(uint32_t first, uint32_t second);

		static const Color RED;
		static const Color GREEN;
		static const Color BLUE;
		static const Color WHITE;
		static const Color BLACK;

		float r;
		float g;
		float b;
		float a;
	};
}
