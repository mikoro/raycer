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
		Image(uint64_t length);
		Image(uint64_t width, uint64_t height);
		Image(uint64_t width, uint64_t height, float* rgbaData);
		Image(const std::string& fileName);

		void load(uint64_t width, uint64_t height, float* rgbaData);
		void load(const std::string& fileName);
		void save(const std::string& fileName) const;
		void resize(uint64_t length);
		void resize(uint64_t width, uint64_t height);
		void setPixel(uint64_t x, uint64_t y, const Color& color);
		void setPixel(uint64_t index, const Color& color);
		void clear();
		void clear(const Color& color);
		void applyFastGamma(double gamma);
		void swapComponents();
		void flip();
		void fillTestPattern();

		uint64_t getWidth() const;
		uint64_t getHeight() const;
		uint64_t getLength() const;

		Color getPixel(uint64_t x, uint64_t y) const;
		Color getPixel(uint64_t index) const;
		Color getPixelNearest(double u, double v) const;
		Color getPixelBilinear(double u, double v) const;

		AlignedColorfVector& getPixelData();
		const AlignedColorfVector& getPixelDataConst() const;

	private:

		uint64_t width = 0;
		uint64_t height = 0;

		AlignedColorfVector pixelData;
	};
}
