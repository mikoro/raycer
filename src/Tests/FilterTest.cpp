// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#ifdef RUN_UNIT_TESTS

#include "catch/catch.hpp"

#include "Rendering/Filters/Filter.h"
#include "Rendering/Filters/BoxFilter.h"
#include "Rendering/Filters/TentFilter.h"
#include "Rendering/Filters/BellFilter.h"
#include "Rendering/Filters/MitchellFilter.h"
#include "Rendering/Filters/GaussianFilter.h"
#include "Rendering/Filters/LanczosSincFilter.h"

using namespace Raycer;

TEST_CASE("Filter functionality", "[filter]")
{
	BoxFilter boxFilter;
	TentFilter tentFilter;
	BellFilter bellFilter;
	MitchellFilter mitchellFilter;
	GaussianFilter gaussianFilter;
	LanczosSincFilter lanczosFilter;

	std::map<std::string, Filter*> filters;
	filters["box"] = &boxFilter;
	filters["tent"] = &tentFilter;
	filters["bell"] = &bellFilter;
	filters["mitchell"] = &mitchellFilter;
	filters["gaussian"] = &gaussianFilter;
	filters["lanczos"] = &lanczosFilter;

	for (const auto &filter : filters)
	{
		std::ofstream file1(tfm::format("filter_%s_1D.txt", filter.first));
		std::ofstream file2(tfm::format("filter_%s_2D.txt", filter.first));

		double extent = 8.0;
		uint steps = 1000;
		double stepSize = extent / steps;

		for (uint i = 0; i < steps; ++i)
		{
			double x = -(extent / 2.0) + i * stepSize;
			file1 << tfm::format("%f %f\n", x, filter.second->getWeight(x));
		}

		steps = 40;
		stepSize = extent / steps;

		for (uint i = 0; i < steps; ++i)
		{
			for (uint j = 0; j < steps; ++j)
			{
				double x = -(extent / 2.0) + j * stepSize;
				double y = -(extent / 2.0) + i * stepSize;
				file2 << tfm::format("%f %f %f\n", x, y, filter.second->getWeight(x, y));
			}
		}

		file1.close();
		file2.close();
	}
}

#endif
