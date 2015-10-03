// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Vector2;
	class Vector3;
	class ONB;

	enum class SamplerType { RANDOM, REGULAR, JITTERED, CMJ };

	class Sampler
	{
	public:

		virtual ~Sampler()
		{
		}

		virtual double getSample(uint i, uint n, uint permutation = 0) = 0;
		virtual Vector2 getSquareSample(uint ix, uint iy, uint nx, uint ny, uint permutation = 0) = 0;
		virtual Vector2 getDiskSample(uint ix, uint iy, uint nx, uint ny, uint permutation = 0) = 0;
		virtual Vector3 getHemisphereSample(const ONB& onb, double distribution, uint ix, uint iy, uint nx, uint ny, uint permutation = 0) = 0;

		static Vector2 mapToDisk(const Vector2& point);
		static Vector3 mapToHemisphere(const ONB& onb, double distribution, const Vector2& point);
	};
}
