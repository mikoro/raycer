// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Rendering/Filters/Filter.h"

namespace Raycer
{
	class BoxFilter : public Filter
	{
	public:

		BoxFilter(double width = 1.0);

		double getWeight(double x);
		double getWeight(double x, double y);
		double getWeight(const Vector2& point);
		double getWidth();
	};
}
