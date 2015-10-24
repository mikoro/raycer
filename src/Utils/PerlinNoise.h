// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

/*

http://mrl.nyu.edu/~perlin/noise/
http://mrl.nyu.edu/~perlin/paper445.pdf

getNoise returns values between 0.0 - 1.0
getFbmNoise return values between 0.0 - inf

*/

namespace Raycer
{
	class PerlinNoise
	{
	public:

		PerlinNoise();
		PerlinNoise(uint32_t seed);

		void seed(uint32_t seed);
		double getNoise(double x, double y, double z) const;
		double getFbmNoise(uint64_t octaves, double lacunarity, double persistence, double x, double y, double z) const;

	private:

		double fade(double t) const;
		double lerp(double t, double a, double b) const;
		double grad(size_t hash, double x, double y, double z) const;

		std::vector<size_t> permutations;
	};
}
