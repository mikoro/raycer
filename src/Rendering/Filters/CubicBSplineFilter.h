// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Rendering/Filters/Filter.h"

namespace Raycer
{
	class CubicBSplineFilter : public Filter
	{
	public:

		CubicBSplineFilter(double B = 1.0 / 3.0, double C = 1.0 / 3.0);

		double getWeight(double x);
		double getWeight(double x, double y);
		double getWeight(const Vector2& point);

	private:

		double B;
		double C;
	};
}
