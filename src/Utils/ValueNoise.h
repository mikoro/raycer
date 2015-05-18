// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class ValueNoise
	{
	public:

		ValueNoise();
		ValueNoise(int seed);

		void seed(int seed);
		double getNoise(double x, double y, double z) const;
		double getFbmNoise(int octaves, double lacunarity, double persistence, double x, double y, double z) const;

	private:

		int getHashcode(int x, int y, int z) const;

		int m_seed;
	};
}
