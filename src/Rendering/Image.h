// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

/*

Origin (0, 0) is at the bottom left corner.

*/

namespace Raycer
{
	class Color;

	class Image
	{
	public:

		Image();
		Image(int length);
		Image(int width, int height);
		Image(int width, int height, float* rgbaData);
		Image(const Image& other);
		Image(const std::string& fileName);
		~Image();

		Image& operator=(const Image& other);

		void load(int width, int height, float* rgbaData);
		void load(const std::string& fileName);
		void saveAs(const std::string& fileName) const;
		void setSize(int length);
		void setSize(int width, int height);
		void setPixel(int x, int y, const Color& color);
		void setPixel(int index, const Color& color);
		void clear();
		void clear(const Color& color);
		void applyGamma(double gamma);
		void swapComponents();
		void flip();
		void fillTestPattern();

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
