// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <random>

/*

http://graphics.ucsd.edu/courses/cse168_s06/ucsd/cellular_noise.pdf

Returns values between 0.0 - 1.0

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
		double getNoise2D(CellNoiseType type, int order, int density, double x, double y) const;
		double getNoise3D(CellNoiseType type, int order, int density, double x, double y, double z) const;
		double getFbmNoise2D(CellNoiseType type, int order, int density, int octaves, double lacunarity, double persistence, double x, double y) const;
		double getFbmNoise3D(CellNoiseType type, int order, int density, int octaves, double lacunarity, double persistence, double x, double y, double z) const;

	private:

		int getHashcode2D(int x, int y) const;
		int getHashcode3D(int x, int y, int z) const;

		static double euclideanDistance2D(const Vector2& v1, const Vector2& v2);
		static double euclideanDistance3D(const Vector3& v1, const Vector3& v2);
		static double euclideanDistanceSquared2D(const Vector2& v1, const Vector2& v2);
		static double euclideanDistanceSquared3D(const Vector3& v1, const Vector3& v2);
		static double manhattanDistance2D(const Vector2& v1, const Vector2& v2);
		static double manhattanDistance3D(const Vector3& v1, const Vector3& v2);
		static double chebyshevDistance2D(const Vector2& v1, const Vector2& v2);
		static double chebyshevDistance3D(const Vector3& v1, const Vector3& v2);

		int m_seed;
	};
}
