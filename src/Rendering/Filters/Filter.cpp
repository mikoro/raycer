// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>

#include "Rendering/Filters/Filter.h"
#include "Rendering/Filters/BoxFilter.h"
#include "Rendering/Filters/TentFilter.h"
#include "Rendering/Filters/BellFilter.h"
#include "Rendering/Filters/CubicBSplineFilter.h"
#include "Rendering/Filters/GaussianFilter.h"
#include "Rendering/Filters/LanczosSincFilter.h"

using namespace Raycer;

Filter* Filter::getFilter(FilterType type)
{
	switch (type)
	{
		case FilterType::BOX: return new BoxFilter();
		case FilterType::TENT: return new TentFilter();
		case FilterType::BELL: return new BellFilter();
		case FilterType::CUBIC_BSPLINE: return new CubicBSplineFilter();
		case FilterType::GAUSSIAN: return new GaussianFilter();
		case FilterType::LANCZOS_SINC: return new LanczosSincFilter();
		default: throw std::runtime_error("Invalid filter type");
	}
}
