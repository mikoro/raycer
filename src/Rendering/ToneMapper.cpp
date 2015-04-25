// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Rendering/ToneMapper.h"
#include "Math/Color.h"

using namespace Raycer;

Color ToneMapper::gamma(const Color& pixelColor, double gamma)
{
	Color result = pixelColor;
	result.a = 1.0;

	return Color::pow(result, gamma).clamped();
}
