// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "catch/catch.hpp"

#include "Math/ColorGradient.h"
#include "Rendering/Image.h"
#include "Math/Color.h"

using namespace Raycer;

TEST_CASE("ColorGradient functionality", "[colorgradient]")
{
	Image image(1000, 1000);
	ColorGradient gradient;

	/*
	gradient.addSegment(Color(0, 0, 0), Color(50, 50, 50), 10);
	gradient.addSegment(Color(60, 60, 60), Color(100, 100, 100), 10);
	gradient.addSegment(Color(110, 110, 110), Color(150, 150, 150), 10);
	gradient.addSegment(Color(160, 160, 160), Color(200, 200, 200), 10);
	*/

	gradient.addSegment(Color(0, 0, 0), Color(0, 0, 0), 40);
	gradient.addSegment(Color(0, 0, 0), Color(75, 75, 75), 10);
	gradient.addSegment(Color(75, 75, 75), Color(255, 0, 0), 10);
	gradient.addSegment(Color(255, 0, 0), Color(255, 255, 0), 40);

	for (int y = 0; y < 1000; ++y)
	{
		double alpha = (double)y / 999.0;
		Color color = gradient.getColor(alpha);

		for (int x = 0; x < 1000; ++x)
			image.setPixel(x, y, color);
	}

	image.saveAs("gradient.png");
}
