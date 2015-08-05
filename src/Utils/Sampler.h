// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <random>

/*

Sample values are within 0.0 .. 1.0, expect disk samples that are -1.0 .. 1.0
Arguments n* denotes the sample count in its direction.
Arguments i* denotes current index of the sample, n* being the maximum.
CMJ samples are generated pseudo-randomly, same permutation value will always return same values.

*/

namespace Raycer
{
	class Vector2;
	class Vector3;

	class Sampler
	{
	public:

		Sampler();

		Vector2 getRandomSample();
		Vector2 getRegularGridSample(int ix, int iy, int nx, int ny);
		Vector2 getJitteredSample(int ix, int iy, int nx, int ny);
		Vector2 getCmjSample(int ix, int iy, int nx, int ny, int permutation);
		Vector2 getCmjDiskSample(int ix, int iy, int nx, int ny, int permutation);
		Vector3 getCmjHemisphereSample(const Vector3& u, const Vector3& v, const Vector3& w, double distribution, int ix, int iy, int nx, int ny, int permutation);

	private:

		std::mt19937 gen;
		std::uniform_real_distribution<double> realDist;
	};
}
