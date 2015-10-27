// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/Sampler.h"
#include "Rendering/Samplers/CenterSampler.h"
#include "Rendering/Samplers/RandomSampler.h"
#include "Rendering/Samplers/RegularSampler.h"
#include "Rendering/Samplers/JitteredSampler.h"
#include "Rendering/Samplers/CMJSampler.h"
#include "Rendering/Samplers/PoissonDiscSampler.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/ONB.h"

using namespace Raycer;

std::unique_ptr<Sampler> Sampler::getSampler(SamplerType type)
{
	switch (type)
	{
		case SamplerType::CENTER: return std::make_unique<CenterSampler>();
		case SamplerType::RANDOM: return std::make_unique<RandomSampler>();
		case SamplerType::REGULAR: return std::make_unique<RegularSampler>();
		case SamplerType::JITTERED: return std::make_unique<JitteredSampler>();
		case SamplerType::CMJ: return std::make_unique<CMJSampler>();
		case SamplerType::POISSON_DISC: return std::make_unique<PoissonDiscSampler>();
		default: throw new std::runtime_error("Unknown sampler type");
	}
}

namespace
{
	Vector2 mapToDisc(const Vector2& point)
	{
		Vector2 result;

		// square to disc polar mapping
		double theta = 2.0 * M_PI * point.x;
		double r = sqrt(point.y);

		result.x = r * cos(theta);
		result.y = r * sin(theta);

		return result;
	}

	Vector3 mapToHemisphere(const ONB& onb, double distribution, const Vector2& point)
	{
		// square to hemisphere mapping with cosine distribution
		double phi = 2.0 * M_PI * point.x;
		double cos_phi = cos(phi);
		double sin_phi = sin(phi);
		double cos_theta = pow(1.0 - point.y, 1.0 / (distribution + 1.0));
		double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

		double u = sin_theta * cos_phi;
		double v = sin_theta * sin_phi;
		double w = cos_theta;

		return u * onb.u + v * onb.v + w * onb.w;
	}
}

Vector2 Sampler::getDiscSample(uint64_t x, uint64_t y, uint64_t nx, uint64_t ny, uint64_t permutation, std::mt19937& generator)
{
	Vector2 point = getSample2D(x, y, nx, ny, permutation, generator);
	return mapToDisc(point);
}

Vector3 Sampler::getHemisphereSample(const ONB& onb, double distribution, uint64_t x, uint64_t y, uint64_t nx, uint64_t ny, uint64_t permutation, std::mt19937& generator)
{
	Vector2 point = getSample2D(x, y, nx, ny, permutation, generator);
	return mapToHemisphere(onb, distribution, point);
}

void Sampler::generateSamples1D(uint64_t sampleCount, std::mt19937& generator)
{
	samples1D.resize(sampleCount);
	std::uniform_int_distribution<uint64_t> randomPermutation;
	uint64_t permutation = randomPermutation(generator);

	for (uint64_t i = 0; i < sampleCount; ++i)
		samples1D[i] = getSample1D(i, sampleCount, permutation, generator);

	nextSampleIndex1D = 0;
}

void Sampler::generateSamples2D(uint64_t sampleCountSqrt, std::mt19937& generator)
{
	samples2D.resize(sampleCountSqrt * sampleCountSqrt);
	std::uniform_int_distribution<uint64_t> randomPermutation;
	uint64_t permutation = randomPermutation(generator);

	for (uint64_t y = 0; y < sampleCountSqrt; ++y)
	{
		for (uint64_t x = 0; x < sampleCountSqrt; ++x)
		{
			samples2D[y * sampleCountSqrt + x] = getSample2D(x, y, sampleCountSqrt, sampleCountSqrt, permutation, generator);
		}
	}

	nextSampleIndex2D = 0;
}

bool Sampler::getNextSample1D(double& result)
{
	if (nextSampleIndex1D >= samples1D.size())
	{
		nextSampleIndex1D = 0;
		return false;
	}

	result = samples1D[nextSampleIndex1D++];
	return true;
}

bool Sampler::getNextSample2D(Vector2& result)
{
	if (nextSampleIndex2D >= samples2D.size())
	{
		nextSampleIndex2D = 0;
		return false;
	}

	result = samples2D[nextSampleIndex2D++];
	return true;
}

bool Sampler::getNextDiscSample(Vector2& result)
{
	if (nextSampleIndex2D >= samples2D.size())
	{
		nextSampleIndex2D = 0;
		return false;
	}

	result = mapToDisc(samples2D[nextSampleIndex2D++]);
	return true;
}

bool Sampler::getNextHemisphereSample(const ONB& onb, double distribution, Vector3& result)
{
	if (nextSampleIndex2D >= samples2D.size())
	{
		nextSampleIndex2D = 0;
		return false;
	}

	result = mapToHemisphere(onb, distribution, samples2D[nextSampleIndex2D++]);
	return true;
}

void Sampler::reset()
{
	nextSampleIndex1D = 0;
	nextSampleIndex2D = 0;
}
