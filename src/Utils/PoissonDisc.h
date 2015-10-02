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
			int width = 0;
			int height = 0;
		};

		struct GridIndex2D
		{
			int x = 0;
			int y = 0;
		};
	}

	class PoissonDisc
	{
	public:

		PoissonDisc();
		PoissonDisc(int seed);

		void seed(int seed);
		void generate2D(int width, int height, double minDistance, int iterationLimit = 30);

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

		std::mt19937 mt;
	};
}
