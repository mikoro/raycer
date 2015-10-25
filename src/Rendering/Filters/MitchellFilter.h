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

		explicit MitchellFilter(double B = (1.0 / 3.0), double C = (1.0 / 3.0));

		void setCoefficients(double B, double C);

		double getWeightX(double x) override;
		double getWeightY(double y) override;

	private:

		double B = 0.0;
		double C = 0.0;
	};
}
