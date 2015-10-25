// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Vector2;

	enum class FilterType { BOX, TENT, BELL, GAUSSIAN, MITCHELL, LANCZOS_SINC };

	class Filter
	{
	public:

		virtual ~Filter() {}

		virtual double getWeightX(double x) = 0;
		virtual double getWeightY(double y) = 0;

		double getWeight(double x, double y);
		double getWeight(const Vector2& point);
		
		double getRadiusX() const;
		double getRadiusY() const;
		Vector2 getRadius() const;

		static std::unique_ptr<Filter> getFilter(FilterType type);

	protected:

		double radiusX = 0.0;
		double radiusY = 0.0;
	};
}
