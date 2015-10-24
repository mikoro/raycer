// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class ValueNoise
	{
	public:

		ValueNoise();
		ValueNoise(uint32_t seed);

		void seed(uint32_t seed);
		double getNoise(double x, double y, double z) const;
		double getFbmNoise(uint64_t octaves, double lacunarity, double persistence, double x, double y, double z) const;

	private:

		uint32_t getHashcode(int64_t x, int64_t y, int64_t z) const;

		uint32_t m_seed;
	};
}
