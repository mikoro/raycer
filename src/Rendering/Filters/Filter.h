// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Vector2;

	enum class FilterType { BOX, TENT, BELL, MITCHELL, GAUSSIAN, LANCZOS_SINC };

	class Filter
	{
	public:

		virtual ~Filter()
		{
		}

		virtual double getWeight(double x) = 0;
		virtual double getWeight(double x, double y) = 0;
		virtual double getWeight(const Vector2& point) = 0;
		virtual double getWidth() = 0;

	protected:

		double width = 1.0;
	};
}
