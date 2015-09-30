// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Utils/ColorGradient.h"

using namespace Raycer;

void ColorGradient::addSegment(const Color& start, const Color& end, int length)
{
	assert(length >= 1);

	ColorGradientSegment segment;

	segment.startColor = start;
	segment.endColor = end;
	segment.startIndex = totalLength;
	segment.endIndex = totalLength + length;

	segments.push_back(segment);

	totalLength += length;
}

Color ColorGradient::getColor(double alpha) const
{
	assert(alpha >= 0.0 && alpha <= 1.0);

	Color result;
	int index = (int)ceil(alpha * (double)totalLength);

	for (const ColorGradientSegment& segment : segments)
	{
		if (index >= segment.startIndex && index <= segment.endIndex)
		{
			double alphaStart = (double)segment.startIndex / (double)totalLength;
			double alphaEnd = (double)segment.endIndex / (double)totalLength;
			double segmentAlpha = (alpha - alphaStart) / (alphaEnd - alphaStart);

			result = Color::lerp(segment.startColor, segment.endColor, segmentAlpha);

			break;
		}
	}

	return result;
}
