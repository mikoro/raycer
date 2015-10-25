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
#include "Rendering/Samplers/PoissonDiscSampler.h"
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
	PoissonDiscSampler poissonDiscSampler;

	std::map<std::string, Sampler*> samplers;
	samplers["random"] = &randomSampler;
	samplers["regular"] = &regularSampler;
	samplers["jittered"] = &jitteredSampler;
	samplers["cmj"] = &cmjSampler;
	samplers["poisson_disc"] = &poissonDiscSampler;

	for (const auto &sampler : samplers)
	{
		uint64_t sampleCount = 32;
		Vector2 size = Vector2(99.0, 99.0);

		Image image1(100, 100);
		Image image2(100, 100);
		Image image3(100, 100);
		
		std::ofstream file(tfm::format("sampler_%s_hemisphere.txt", sampler.first));

		std::random_device rd;
		sampler.second->setPermutation(rd());
		sampler.second->generateSamples1D(sampleCount);
		sampler.second->generateSamples2D(sampleCount);

		double sample1D;

		while (sampler.second->getNextSample1D(sample1D))
		{
			sample1D *= size.x;
			image1.setPixel(uint64_t(sample1D + 0.5), uint64_t(size.y / 2.0 + 0.5), Color(255, 255, 255));
		}

		Vector2 sample2D;

		while (sampler.second->getNextSample2D(sample2D))
		{
			sample2D *= size;
			image2.setPixel(uint64_t(sample2D.x + 0.5), uint64_t(sample2D.y + 0.5), Color(255, 255, 255));
		}

		while (sampler.second->getNextDiscSample(sample2D))
		{
			sample2D = (sample2D / 2.0 + Vector2(0.5, 0.5)) * size;
			image3.setPixel(uint64_t(sample2D.x + 0.5), uint64_t(sample2D.y + 0.5), Color(255, 255, 255));
		}

		Vector3 sample3D;

		while (sampler.second->getNextHemisphereSample(ONB::UP, 1.0, sample3D))
			file << tfm::format("%f %f %f\n", sample3D.x, sample3D.y, sample3D.z);

		image1.save(tfm::format("sampler_%s_1D.png", sampler.first));
		image2.save(tfm::format("sampler_%s_2D.png", sampler.first));
		image3.save(tfm::format("sampler_%s_disc.png", sampler.first));

		file.close();
	}
}

#endif
