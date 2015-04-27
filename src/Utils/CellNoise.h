// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

/*

http://graphics.ucsd.edu/courses/cse168_s06/ucsd/cellular_noise.pdf

Returns sorted distances to all neighbours (non-normalized)

*/

namespace Raycer
{
	enum class CellNoiseType { EUCLIDEAN, EUCLIDEAN_SQUARED, MANHATTAN, CHEBYSHEV };

	class Vector2;
	class Vector3;

	class CellNoise
	{
	public:

		CellNoise();
		CellNoise(int seed);

		void seed(int seed);

		std::vector<double> getNoise(CellNoiseType type, int density, double x, double y, double z) const;

	private:

		int getHashcode(int x, int y, int z) const;

		static double euclideanDistance(const Vector3& v1, const Vector3& v2);
		static double euclideanDistanceSquared(const Vector3& v1, const Vector3& v2);
		static double manhattanDistance(const Vector3& v1, const Vector3& v2);
		static double chebyshevDistance(const Vector3& v1, const Vector3& v2);

		int m_seed;
	};
}
