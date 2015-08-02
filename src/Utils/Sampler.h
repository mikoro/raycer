// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <random>

/*

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

		Vector2 getCentroidSample(const Vector2& origin);
		Vector2 getRandomSample(const Vector2& origin);
		Vector2 getRegularGridSample(const Vector2& origin, int ix, int iy, int nx, int ny);
		Vector2 getJitteredSample(const Vector2& origin, int ix, int iy, int nx, int ny);
		Vector2 getCmjSample(const Vector2& origin, int ix, int iy, int nx, int ny, int permutation);
		Vector2 getCmjDiskSample(const Vector2& origin, int ix, int iy, int nx, int ny, int permutation);
		Vector3 getHemisphericalSample(const Vector3& u, const Vector3& v, const Vector3& w, double distribution, int ix, int iy, int nx, int ny, int permutation);

	private:

		std::mt19937 gen;
		std::uniform_real_distribution<double> realDist;
	};
}
