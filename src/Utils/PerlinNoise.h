// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

/*

http://mrl.nyu.edu/~perlin/noise/
http://mrl.nyu.edu/~perlin/paper445.pdf

*/

namespace Raycer
{
	class PerlinNoise
	{
	public:

		PerlinNoise(int seed);

		double getNoise(double x, double y = 0.0, double z = 0.0) const;
		double getOctaveNoise(int octaves, double persistence, double x, double y = 0.0, double z = 0.0) const;

	private:

		double fade(double t) const;
		double lerp(double t, double a, double b) const;
		double grad(int hash, double x, double y, double z) const;

		std::vector<int> permutations;
	};
}
