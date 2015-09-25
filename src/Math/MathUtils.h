// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <complex>
#include <limits>

namespace Raycer
{
	class MathUtils
	{
	public:

		static bool almostZero(double value, double threshold = std::numeric_limits<double>::epsilon() * 4);
		static bool almostSame(double first, double second, double threshold = std::numeric_limits<double>::epsilon() * 4);
		static bool almostSame(std::complex<double> first, std::complex<double> second, double threshold = std::numeric_limits<double>::epsilon() * 4);
		static double degToRad(double degrees);
		static double radToDeg(double radians);
		static double smoothstep(double t);
		static double smootherstep(double t);
		static double fastPow(double a, double b);
	};
}
