// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class MathUtils
	{
	public:

		static bool almostZero(float value, int epsilons = 1);
		static bool almostSame(float first, float second, int epsilons = 1);
	};
}
