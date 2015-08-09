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

		int startIndex = 0;
		int endIndex = 0;
	};

	class ColorGradient
	{
	public:

		void addSegment(const Color& start, const Color& end, int length);

		Color getColor(double alpha) const;

	private:

		int totalLength = 0;

		std::vector<ColorGradientSegment> segments;
	};
}
