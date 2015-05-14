// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

/*

http://graphics.ucsd.edu/courses/cse168_s06/ucsd/cellular_noise.pdf

Returns distance to closest neighbour (non-normalized) 0.0 - inf

*/

namespace Raycer
{
	enum class CellNoiseDistanceType { EUCLIDEAN, EUCLIDEAN_SQUARED, MANHATTAN, CHEBYSHEV };
	enum class CellNoiseCombineType { D1, D2, D1_PLUS_D2, D1_MINUS_D2, D2_MINUS_D1 };

	class Vector2;
	class Vector3;

	class CellNoise
	{
	public:

		CellNoise();
		CellNoise(int seed);

		void seed(int seed);

		double getNoise(CellNoiseDistanceType distanceType, CellNoiseCombineType combineType, int density, double x, double y, double z) const;

	private:

		int getHashcode(int x, int y, int z) const;

		static double euclideanDistance(const Vector3& v1, const Vector3& v2);
		static double euclideanDistanceSquared(const Vector3& v1, const Vector3& v2);
		static double manhattanDistance(const Vector3& v1, const Vector3& v2);
		static double chebyshevDistance(const Vector3& v1, const Vector3& v2);

		int m_seed;
	};
}
