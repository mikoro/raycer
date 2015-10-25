// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Rendering/Filters/Filter.h"

namespace Raycer
{
	class Vector2;

	class TentFilter : public Filter
	{
	public:

		explicit TentFilter(double radiusX = 1.0, double radiusY = 1.0);

		void setRadius(double radiusX, double radiusY);

		double getWeightX(double x) override;
		double getWeightY(double y) override;

	private:

		double radiusXInv = 0.0;
		double radiusYInv = 0.0;
	};
}
