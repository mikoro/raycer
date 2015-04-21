// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Color;

	class Texture
	{
		public:

			virtual void initialize() = 0;
			virtual Color getColor(double u, double v) const = 0;

			int id = 0;
	};
}
