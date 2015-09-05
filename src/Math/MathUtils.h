// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <complex>

namespace Raycer
{
	class MathUtils
	{
	public:

		static bool almostZero(double value, int epsilons = 4);
		static bool almostSame(double first, double second, int epsilons = 4);
		static bool almostSame(std::complex<double> first, std::complex<double> second, int epsilons = 4);
		static double degToRad(double degrees);
		static double radToDeg(double radians);
		static double smoothstep(double t);
		static double smootherstep(double t);
	};
}
