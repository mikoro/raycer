// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Math/Vector2.h"

/*

Samplers work in procedual and/or pre-generated fashion.
getSample generates a new sample on the fly
generateSamples fills internal buffer with getSample or some other way
getNextSample loops through the internal buffer

*/

namespace Raycer
{
	class Vector3;
	class ONB;

	enum class SamplerType { RANDOM, REGULAR, JITTERED, CMJ };

	class Sampler
	{
	public:

		Sampler();
		virtual ~Sampler() {}

		void setPermutation(uint permutation);

		virtual double getSample1D(uint x, uint n) = 0;
		virtual Vector2 getSample2D(uint x, uint y, uint nx, uint ny) = 0;
		Vector2 getDiskSample(uint x, uint y, uint nx, uint ny);
		Vector3 getHemisphereSample(const ONB& onb, double distribution, uint x, uint y, uint nx, uint ny);

		virtual void generateSamples1D(uint sampleCount);
		virtual void generateSamples2D(uint sampleCountSqrt);

		bool getNextSample1D(double& result);
		bool getNextSample2D(Vector2& result);
		bool getNextDiskSample(Vector2& result);
		bool getNextHemisphereSample(const ONB& onb, double distribution, Vector3& result);

		static std::unique_ptr<Sampler> getSampler(SamplerType type);

	protected:

		uint permutation = 0;

		size_t currentSampleIndex1D = 0;
		size_t currentSampleIndex2D = 0;

		std::vector<double> samples1D;
		std::vector<Vector2> samples2D;
	};
}
