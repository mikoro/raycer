// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Math/Color.h"

/*

Origin (0, 0) is at the bottom left corner.

*/

namespace Raycer
{
	class Image
	{
	public:

		Image();
		Image(size_t length);
		Image(size_t width, size_t height);
		Image(size_t width, size_t height, float* rgbaData);
		Image(const std::string& fileName);

		void load(size_t width, size_t height, float* rgbaData);
		void load(const std::string& fileName);
		void save(const std::string& fileName) const;
		void resize(size_t length);
		void resize(size_t width, size_t height);
		void setPixel(size_t x, size_t y, const Color& color);
		void setPixel(size_t index, const Color& color);
		void clear();
		void clear(const Color& color);
		void applyFastGamma(double gamma);
		void swapComponents();
		void flip();
		void fillTestPattern();

		size_t getWidth() const;
		size_t getHeight() const;
		size_t getLength() const;

		Color getPixel(size_t x, size_t y) const;
		Color getPixel(size_t index) const;
		Color getPixelNearest(double u, double v) const;
		Color getPixelBilinear(double u, double v) const;

		AlignedColorfVector& getPixelData();
		const AlignedColorfVector& getPixelDataConst() const;

	private:

		size_t width = 0;
		size_t height = 0;

		AlignedColorfVector pixelData;
	};
}
