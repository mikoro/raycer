// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Math/Color.h"

namespace Raycer
{
	struct ColorGradientSegment
	{
		Color startColor;
		Color endColor;

		uint64_t startIndex = 0;
		uint64_t endIndex = 0;
	};

	class ColorGradient
	{
	public:

		void addSegment(const Color& start, const Color& end, uint64_t length);

		Color getColor(double alpha) const;

	private:

		uint64_t totalLength = 0;

		std::vector<ColorGradientSegment> segments;
	};
}
