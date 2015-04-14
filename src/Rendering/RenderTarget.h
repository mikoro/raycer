// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Color;

	class RenderTarget
	{
	public:

		virtual int getWidth() const = 0;
		virtual int getHeight() const = 0;

		virtual Color getPixel(int x, int y) const = 0;
		virtual void setPixel(int x, int y, const Color& color) = 0;
		virtual void setPixel(int index, const Color& color) = 0;
	};
}
