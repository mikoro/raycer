// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Color;

	class RenderTarget
	{
	public:

		virtual size_t getWidth() const = 0;
		virtual size_t getHeight() const = 0;

		virtual Color getPixel(size_t x, size_t y) const = 0;
		virtual void setPixel(size_t x, size_t y, const Color& color) = 0;
		virtual void setPixel(size_t index, const Color& color) = 0;
	};
}
