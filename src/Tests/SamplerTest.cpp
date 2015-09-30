// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#ifdef RUN_UNIT_TESTS

#include "Rendering/Samplers/Sampler.h"
#include "Rendering/Samplers/RandomSampler.h"
#include "Rendering/Samplers/RegularSampler.h"
#include "Rendering/Samplers/JitteredSampler.h"
#include "Rendering/Samplers/CMJSampler.h"
#include "Rendering/Image.h"
#include "Raytracing/ONB.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

using namespace Raycer;

TEST_CASE("Sampler functionality", "[sampler]")
{
	RandomSampler randomSampler;
	RegularSampler regularSampler;
	JitteredSampler jitteredSampler;
	CMJSampler cmjSampler;

	std::map<std::string, Sampler*> samplers;
	samplers["random"] = &randomSampler;
	samplers["regular"] = &regularSampler;
	samplers["jittered"] = &jitteredSampler;
	samplers["cmj"] = &cmjSampler;

	for (const auto &sampler : samplers)
	{
		int n = 32;
		std::random_device rd;
		int permutation = rd();
		Image image1(100, 100);
		Image image2(100, 100);
		Image image3(100, 100);
		Vector2 size = Vector2(99.0, 99.0);
		std::ofstream file(tfm::format("sampler_%s_hemisphere.txt", sampler.first));

		for (int x = 0; x < n; ++x)
		{
			double sx = sampler.second->getSample(x, n, permutation) * size.x;
			image1.setPixel((int)(sx + 0.5), (int)(size.y / 2.0 + 0.5), Color(255, 255, 255));
		}

		for (int y = 0; y < n; ++y)
		{
			for (int x = 0; x < n; ++x)
			{
				Vector2 sample = sampler.second->getSquareSample(x, y, n, n, permutation) * size;
				image2.setPixel((int)(sample.x + 0.5), (int)(sample.y + 0.5), Color(255, 255, 255));

				sample = sampler.second->getDiskSample(x, y, n, n, permutation);
				sample = (sample / 2.0 + Vector2(0.5, 0.5)) * size;
				image3.setPixel((int)(sample.x + 0.5), (int)(sample.y + 0.5), Color(255, 255, 255));

				Vector3 hemiSample = sampler.second->getHemisphereSample(ONB::UP, 1.0, x, y, n, n, permutation);
				file << tfm::format("%f %f %f\n", hemiSample.x, hemiSample.y, hemiSample.z);
			}
		}

		image1.save(tfm::format("sampler_%s_1D.png", sampler.first));
		image2.save(tfm::format("sampler_%s_2D.png", sampler.first));
		image3.save(tfm::format("sampler_%s_disk.png", sampler.first));
		file.close();
	}
}

#endif
