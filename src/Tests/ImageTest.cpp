// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "catch/catch.hpp"

#include "Rendering/Image.h"
#include "Math/Color.h"

using namespace Raycer;

TEST_CASE("Image functionality", "[image]")
{
	Image image(100, 100);

	for (int y = 0; y < 100; ++y)
	{
		for (int x = 0; x < 100; ++x)
		{
			if (y < 50)
				image.setPixel(x, y, Color(10, 100, 200, 255));
			else
				image.setPixel(x, y, Color(255, 0, 0, 128));
		}
	}

	image.saveAs("test1.png");
}
