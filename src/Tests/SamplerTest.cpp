// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "catch/catch.hpp"

#include "Utils/Sampler.h"
#include "Rendering/Image.h"
#include "Math/Color.h"
#include "Math/Vector2.h"

using namespace Raycer;

TEST_CASE("Sampler functionality", "[sampler]")
{
	Image image1(100, 100);
	Image image2(100, 100);
	Image image3(100, 100);
	Image image4(100, 100);
	Image image5(100, 100);
	Image image6(100, 100);
	Sampler sampler;

	int n = 16;
	Vector2 origin = Vector2(0.0, 0.0);
	Vector2 size = Vector2(99.0, 99.0);
	Vector2 sample;

	int permutation = 3465566;

	for (int y = 0; y < n; ++y)
	{
		for (int x = 0; x < n; ++x)
		{
			sample = sampler.getCentroidSample(origin) * size;
			image1.setPixel((int)(sample.x + 0.5), (int)(sample.y + 0.5), Color(255, 255, 255));

			sample = sampler.getRandomSample(origin) * size;
			image2.setPixel((int)(sample.x + 0.5), (int)(sample.y + 0.5), Color(255, 255, 255));

			sample = sampler.getRegularGridSample(origin, x, y, n, n) * size;
			image3.setPixel((int)(sample.x + 0.5), (int)(sample.y + 0.5), Color(255, 255, 255));

			sample = sampler.getJitteredSample(origin, x, y, n, n) * size;
			image4.setPixel((int)(sample.x + 0.5), (int)(sample.y + 0.5), Color(255, 255, 255));

			sample = sampler.getCmjSample(origin, x, y, n, n, permutation) * size;
			image5.setPixel((int)(sample.x + 0.5), (int)(sample.y + 0.5), Color(255, 255, 255));

			sample = sampler.getCmjDiskSample(origin, x, y, n, n, permutation) * size;
			image6.setPixel((int)(sample.x + 0.5), (int)(sample.y + 0.5), Color(255, 255, 255));
		}
	}

	image1.saveAs("sampler_centroid.png");
	image2.saveAs("sampler_random.png");
	image3.saveAs("sampler_regular_grid.png");
	image4.saveAs("sampler_jittered.png");
	image5.saveAs("sampler_cmj.png");
	image6.saveAs("sampler_cmj_disk.png");
}
