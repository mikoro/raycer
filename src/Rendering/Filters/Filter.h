// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

/*

All inputs are in -1.0 .. 1.0, outside values will return 0.0
Outputs are in 0.0 .. 1.0

*/

namespace Raycer
{
	class Vector2;

	enum class FilterType { BOX, TENT, BELL, CUBIC_BSPLINE, GAUSSIAN, LANCZOS_SINC };

	class Filter
	{
	public:

		virtual ~Filter() {}

		virtual double getWeight(double x) = 0;
		virtual double getWeight(double x, double y) = 0;
		virtual double getWeight(const Vector2& point) = 0;

		static Filter* getFilter(FilterType type);
	};
}
