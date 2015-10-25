// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Rendering/Filters/Filter.h"

namespace Raycer
{
	class Vector2;

	class BoxFilter : public Filter
	{
	public:

		explicit BoxFilter(double radiusX = 0.5, double radiusY = 0.5);

		void setRadius(double radiusX, double radiusY);

		double getWeightX(double x) override;
		double getWeightY(double y) override;

	private:

		double weightX = 0.0;
		double weightY = 0.0;
	};
}
