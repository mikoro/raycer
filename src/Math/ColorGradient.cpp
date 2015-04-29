// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cassert>

#include "Math/ColorGradient.h"

using namespace Raycer;

void ColorGradient::addSegment(const Color& start, const Color& end, int length)
{
	assert(length >= 1);

	ColorGradientSegment segment;

	segment.startColor = start;
	segment.endColor = end;
	segment.startIndex = totalLength;
	segment.endIndex = (totalLength += length);

	segments.push_back(segment);
}

Color ColorGradient::getColor(double alpha)
{
	assert(alpha >= 0.0 && alpha <= 1.0);

	Color result;
	int index = (int)(alpha * totalLength + 0.5);

	for (ColorGradientSegment& segment : segments)
	{
		if (index >= segment.startIndex && index <= segment.endIndex)
		{
			int segmentLength = segment.endIndex - segment.startIndex;
			int segmentIndex = index - segment.startIndex;
			double interpolation = (double)segmentIndex / (double)segmentLength;

			result = Color::lerp(segment.startColor, segment.endColor, interpolation);

			break;
		}
	}

	return result;
}
