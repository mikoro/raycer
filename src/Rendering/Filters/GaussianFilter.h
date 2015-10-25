// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Rendering/Filters/Filter.h"

namespace Raycer
{
	class Vector2;

	class GaussianFilter : public Filter
	{
	public:

		explicit GaussianFilter(double stdDevX = 1.0, double stdDevY = 1.0);

		void setStandardDeviations(double stdDevX, double stdDevY);

		double getWeightX(double x) override;
		double getWeightY(double y) override;

	private:

		double alphaX = 0.0;
		double alphaY = 0.0;
		double betaX = 0.0;
		double betaY = 0.0;
	};
}
