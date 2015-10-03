// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class ValueNoise
	{
	public:

		ValueNoise();
		ValueNoise(uint seed);

		void seed(uint seed);
		double getNoise(double x, double y, double z) const;
		double getFbmNoise(uint octaves, double lacunarity, double persistence, double x, double y, double z) const;

	private:

		int getHashcode(int x, int y, int z) const;

		uint m_seed;
	};
}
