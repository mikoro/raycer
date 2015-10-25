// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Filters/Filter.h"
#include "Rendering/Filters/BoxFilter.h"
#include "Rendering/Filters/TentFilter.h"
#include "Rendering/Filters/BellFilter.h"
#include "Rendering/Filters/GaussianFilter.h"
#include "Rendering/Filters/MitchellFilter.h"
#include "Rendering/Filters/LanczosSincFilter.h"
#include "Math/Vector2.h"

using namespace Raycer;

double Filter::getWeight(double x, double y)
{
	return getWeightX(x) * getWeightY(y);
}

double Filter::getWeight(const Vector2& point)
{
	return getWeightX(point.x) * getWeightY(point.y);
}

double Filter::getRadiusX() const
{
	return radiusX;
}

double Filter::getRadiusY() const
{
	return radiusY;
}

Vector2 Filter::getRadius() const
{
	return Vector2(radiusX, radiusY);
}

std::unique_ptr<Filter> Filter::getFilter(FilterType type)
{
	switch (type)
	{
		case FilterType::BOX: return std::make_unique<BoxFilter>();
		case FilterType::TENT: return std::make_unique<TentFilter>();
		case FilterType::BELL: return std::make_unique<BellFilter>();
		case FilterType::GAUSSIAN: return std::make_unique<GaussianFilter>();
		case FilterType::MITCHELL: return std::make_unique<MitchellFilter>();
		case FilterType::LANCZOS_SINC: return std::make_unique<LanczosSincFilter>();
		default: throw new std::runtime_error("Unknown filter type");
	}
}
