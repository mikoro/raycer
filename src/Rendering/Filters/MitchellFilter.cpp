// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Filters/MitchellFilter.h"

using namespace Raycer;

namespace
{
	double calculateWeight(double s, double B, double C)
	{
		s = std::abs(s);

		if (s <= 1.0)
			return ((12.0 - 9.0 * B - 6.0 * C) * (s * s * s) + (-18.0 + 12.0 * B + 6.0 * C) * (s * s) + (6.0 - 2.0 * B)) * (1.0 / 6.0);
		else if (s <= 2.0)
			return ((-B - 6.0 * C) * (s * s * s) + (6.0 * B + 30.0 * C) * (s * s) + (-12.0 * B - 48.0 * C) * s + (8.0 * B + 24.0 * C)) * (1.0 / 6.0);
		else
			return 0.0;
	}
}

MitchellFilter::MitchellFilter(double B_, double C_)
{
	setCoefficients(B_, C_);
}

void MitchellFilter::setCoefficients(double B_, double C_)
{
	B = B_;
	C = C_;
	radiusX = 2.0;
	radiusY = 2.0;
}

double MitchellFilter::getWeightX(double x)
{
	return calculateWeight(x, B, C);
}

double MitchellFilter::getWeightY(double y)
{
	return calculateWeight(y, B, C);
}
