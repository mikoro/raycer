// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Rendering/Filters/Filter.h"

namespace Raycer
{
	class Vector2;

	class MitchellFilter : public Filter
	{
	public:

		MitchellFilter(double width = 2.0, double B = 1.0 / 3.0, double C = 1.0 / 3.0);

		double getWeight(double x);
		double getWeight(double x, double y);
		double getWeight(const Vector2& point);
		double getWidth();

	private:

		double B;
		double C;
	};
}
