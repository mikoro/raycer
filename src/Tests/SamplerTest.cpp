// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <fstream>

#include "catch/catch.hpp"
#include "tinyformat/tinyformat.h"

#include "Utils/Sampler.h"
#include "Rendering/Image.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

using namespace Raycer;

TEST_CASE("Sampler functionality", "[sampler]")
{
	Image image2(100, 100);
	Image image3(100, 100);
	Image image4(100, 100);
	Image image5(100, 100);
	Image image6(100, 100);
	Image image7(100, 100);
	Sampler sampler;

	int n = 8;
	Vector2 size = Vector2(99.0, 99.0);

	int permutation = 3465566;

	std::ofstream file("sampler_hemisphere.txt");

	for (int y = 0; y < n; ++y)
	{
		for (int x = 0; x < n; ++x)
		{
			Vector2 sample = sampler.getRandomSample() * size;
			image2.setPixel((int)(sample.x + 0.5), (int)(sample.y + 0.5), Color(255, 255, 255));

			sample = sampler.getRegularSample(x, y, n, n) * size;
			image3.setPixel((int)(sample.x + 0.5), (int)(sample.y + 0.5), Color(255, 255, 255));

			sample = sampler.getJitteredSample(x, y, n, n) * size;
			image4.setPixel((int)(sample.x + 0.5), (int)(sample.y + 0.5), Color(255, 255, 255));

			sample = sampler.getCmjSample(x, y, n, n, permutation) * size;
			image5.setPixel((int)(sample.x + 0.5), (int)(sample.y + 0.5), Color(255, 255, 255));

			sample = sampler.getCmjDiskSample(x, y, n, n, permutation);
			sample = (sample / 2.0 + Vector2(0.5, 0.5)) * size;
			image6.setPixel((int)(sample.x + 0.5), (int)(sample.y + 0.5), Color(255, 255, 255));

			Vector3 hemiSample = sampler.getCmjHemisphereSample(Vector3::RIGHT, Vector3::UP, Vector3::FORWARD, 1.0, x, y, n, n, permutation);

			file << tfm::format("%f %f %f\n", hemiSample.x, hemiSample.y, hemiSample.z);
		}
	}

	for (int i = 0; i < n; ++i)
	{
		double sample = sampler.getJitteredSample(i, n) * size.x;
		image7.setPixel((int)(sample + 0.5), (int)(size.y / 2.0 + 0.5), Color(255, 255, 255));
	}

	file.close();

	image2.saveAs("sampler_random.png");
	image3.saveAs("sampler_regular.png");
	image4.saveAs("sampler_jittered.png");
	image5.saveAs("sampler_cmj.png");
	image6.saveAs("sampler_cmj_disk.png");
	image7.saveAs("sampler_jittered1D.png");
}
