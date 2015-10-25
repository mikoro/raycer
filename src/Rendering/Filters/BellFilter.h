// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Rendering/Filters/Filter.h"

namespace Raycer
{
	class Vector2;

	class BellFilter : public Filter
	{
	public:

		BellFilter();

		double getWeightX(double x) override;
		double getWeightY(double y) override;
	};
}
