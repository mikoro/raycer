// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/ToneMapper.h"
#include "Math/Color.h"

using namespace Raycer;

Color ToneMapper::apply(const Color& pixelColor)
{
	Color result = pixelColor;
	result.a = 1.0;

	return Color::pow(result, gamma);
}
