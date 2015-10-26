// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Math/Vector2.h"

/*

Samplers work in procedual and/or pre-generated fashion.
getSample generates a new sample on the fly
generateSamples fills internal buffer with getSample or some other way
getNextSample loops through the internal buffer and returns false when one loop through the samples is completed

*/

namespace Raycer
{
	class Vector3;
	class ONB;

	enum class SamplerType { RANDOM, REGULAR, JITTERED, CMJ, POISSON_DISC };

	class Sampler
	{
	public:

		Sampler();
		virtual ~Sampler() {}

		void setPermutation(uint64_t permutation);
		virtual double getSample1D(uint64_t x, uint64_t n) = 0;
		virtual Vector2 getSample2D(uint64_t x, uint64_t y, uint64_t nx, uint64_t ny) = 0;
		Vector2 getDiscSample(uint64_t x, uint64_t y, uint64_t nx, uint64_t ny);
		Vector3 getHemisphereSample(const ONB& onb, double distribution, uint64_t x, uint64_t y, uint64_t nx, uint64_t ny);

		virtual void generateSamples1D(uint64_t sampleCount);
		virtual void generateSamples2D(uint64_t sampleCountSqrt);

		bool getNextSample1D(double& result);
		bool getNextSample2D(Vector2& result);
		bool getNextDiscSample(Vector2& result);
		bool getNextHemisphereSample(const ONB& onb, double distribution, Vector3& result);
		void reset();

		static std::unique_ptr<Sampler> getSampler(SamplerType type);

	protected:

		uint64_t permutation = 0;

		uint64_t nextSampleIndex1D = 0;
		uint64_t nextSampleIndex2D = 0;

		std::vector<double> samples1D;
		std::vector<Vector2> samples2D;
	};
}
