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

		GaussianFilter(double width = 2.0, double alpha = 2.0);

		double getWeight(double x) override;
		double getWeight(double x, double y) override;
		double getWeight(const Vector2& point) override;
		double getWidth() override;

	private:

		double alpha;
	};
}
