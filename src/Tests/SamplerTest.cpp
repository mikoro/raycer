// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#ifdef RUN_UNIT_TESTS

#include "catch/catch.hpp"

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
		uint64_t n = 32;
		std::random_device rd;
		uint64_t permutation = rd();
		Image image1(100, 100);
		Image image2(100, 100);
		Image image3(100, 100);
		Vector2 size = Vector2(99.0, 99.0);
		std::ofstream file(tfm::format("sampler_%s_hemisphere.txt", sampler.first));

		sampler.second->setPermutation(permutation);

		for (uint64_t x = 0; x < n; ++x)
		{
			double sx = sampler.second->getSample1D(x, n) * size.x;
			image1.setPixel(uint64_t(sx + 0.5), uint64_t(size.y / 2.0 + 0.5), Color(255, 255, 255));
		}

		for (uint64_t y = 0; y < n; ++y)
		{
			for (uint64_t x = 0; x < n; ++x)
			{
				Vector2 sample = sampler.second->getSample2D(x, y, n, n) * size;
				image2.setPixel(uint64_t(sample.x + 0.5), uint64_t(sample.y + 0.5), Color(255, 255, 255));

				sample = sampler.second->getDiskSample(x, y, n, n);
				sample = (sample / 2.0 + Vector2(0.5, 0.5)) * size;
				image3.setPixel(uint64_t(sample.x + 0.5), uint64_t(sample.y + 0.5), Color(255, 255, 255));

				Vector3 hemiSample = sampler.second->getHemisphereSample(ONB::UP, 1.0, x, y, n, n);
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
