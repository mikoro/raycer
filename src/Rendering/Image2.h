// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

#include "Rendering/RenderTarget.h"

/*

Origin (0, 0) is at the bottom left corner.

*/

namespace Raycer
{
	class Color;

	class Image2 : public RenderTarget
	{
	public:

		Image2();
		Image2(int length);
		Image2(int width, int height);
		Image2(const Image2& other);
		Image2(const std::string& fileName);
		~Image2();

		Image2& operator=(const Image2& other);

		void load(const std::string& fileName);
		void saveAs(const std::string& fileName) const;
		void setSize(int length);
		void setSize(int width, int height);
		void setPixel(int x, int y, const Color& color);
		void setPixel(int index, const Color& color);
		void clear(const Color& color);
		void applyGamma(double gamma);
		void swapComponents();
		void flip();

		int getWidth() const;
		int getHeight() const;
		int getLength() const;

		Color getPixel(int x, int y) const;
		Color getPixelNearest(double u, double v) const;
		Color getPixelLinear(double u, double v) const;

		int width = 0;
		int height = 0;
		int length = 0;

		Color* pixelData = nullptr;
	};
}
