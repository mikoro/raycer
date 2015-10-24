// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <random>
#include <vector>

#include "Math/Vector2.h"

namespace Raycer
{
	namespace Poisson
	{
		struct Cell2D
		{
			Vector2 point;
			bool hasPoint = false;
		};

		struct Grid2D
		{
			std::vector<std::vector<Cell2D>> grid;
			double cellSize = 0.0;
			size_t width = 0;
			size_t height = 0;
		};

		struct GridIndex2D
		{
			size_t x = 0;
			size_t y = 0;
		};
	}

	class PoissonDisc
	{
	public:

		PoissonDisc();
		PoissonDisc(uint32_t seed);

		void seed(uint32_t seed);
		void generate2D(size_t width, size_t height, double minDistance, uint64_t iterationLimit = 30);

		std::vector<Vector2>& getPoints2D();

	private:

		Poisson::GridIndex2D getGridIndex2D(const Vector2& point);
		Poisson::Cell2D& getGridCell2D(const Vector2& point);
		Vector2 getNextActivePoint2D();
		Vector2 generateNewPoint2D(const Vector2& origin, double minDistance);
		bool isInNeighbourhood2D(const Vector2& point, double minDistance);

		Poisson::Grid2D grid2D;

		std::vector<Vector2> points2D;
		std::vector<Vector2> activePoints2D;

		std::mt19937 generator;
	};
}
