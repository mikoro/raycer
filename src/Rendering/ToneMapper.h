// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Color;

	class ToneMapper
	{
		public:

			static Color gamma(const Color& pixelColor, double gamma);
	};
}
