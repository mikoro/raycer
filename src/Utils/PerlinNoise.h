// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

/*

http://mrl.nyu.edu/~perlin/noise/
http://mrl.nyu.edu/~perlin/paper445.pdf

Returns values between 0.0 - 1.0

*/

namespace Raycer
{
	class PerlinNoise
	{
	public:

		PerlinNoise();
		PerlinNoise(int seed);

		void seed(int seed);
		double getNoise(double x, double y, double z) const;
		double getFbmNoise(int octaves, double lacunarity, double persistence, double x, double y, double z) const;

	private:

		double fade(double t) const;
		double lerp(double t, double a, double b) const;
		double grad(int hash, double x, double y, double z) const;

		std::vector<int> permutations;
	};
}
