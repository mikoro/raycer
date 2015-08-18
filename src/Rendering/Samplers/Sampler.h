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

		virtual ~Sampler() {}

		virtual double getSample1D(int i, int n, int permutation = 0) = 0;
		virtual Vector2 getSample2D(int ix, int iy, int nx, int ny, int permutation = 0) = 0;
		virtual Vector2 getSampleDisk(int ix, int iy, int nx, int ny, int permutation = 0) = 0;
		virtual Vector3 getSampleHemisphere(const ONB& onb, double distribution, int ix, int iy, int nx, int ny, int permutation = 0) = 0;

		static Vector2 mapToDisk(const Vector2& point);
		static Vector3 mapToHemisphere(const ONB& onb, double distribution, const Vector2& point);
	};
}
