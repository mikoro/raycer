// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Utils/PoissonDisc.h"

using namespace Raycer;
using namespace Poisson;

PoissonDisc::PoissonDisc()
{
	std::random_device rd;
	seed(rd());
}

PoissonDisc::PoissonDisc(int seed_)
{
	seed(seed_);
}

void PoissonDisc::seed(int seed)
{
	mt.seed(seed);
}

void PoissonDisc::generate2D(size_t width, size_t height, double minDistance, size_t iterationLimit)
{
	std::uniform_real_distribution<double> random(0.0, 1.0);

	grid2D.cellSize = minDistance / M_SQRT2;
	grid2D.width = size_t(ceil(double(width) / grid2D.cellSize));
	grid2D.height = size_t(ceil(double(height) / grid2D.cellSize));

	grid2D.grid.resize(grid2D.height);

	for (auto& row : grid2D.grid)
		row.resize(grid2D.width);

	points2D.clear();
	activePoints2D.clear();

	Vector2 firstPoint = Vector2(random(mt) * double(width), random(mt) * double(height));

	points2D.push_back(firstPoint);
	activePoints2D.push_back(firstPoint);

	Cell2D& cell1 = getGridCell2D(firstPoint);
	cell1.point = firstPoint;
	cell1.hasPoint = true;

	while (!activePoints2D.empty())
	{
		Vector2 origin = getNextActivePoint2D();

		for (size_t i = 0; i < iterationLimit; ++i)
		{
			Vector2 point = generateNewPoint2D(origin, minDistance);

			if (point.x < 0.0 || point.x > double(width) || point.y < 0.0 || point.y > double(height))
				continue;

			if (isInNeighbourhood2D(point, minDistance))
				continue;

			points2D.push_back(point);
			activePoints2D.push_back(point);

			Cell2D& cell2 = getGridCell2D(point);
			cell2.point = point;
			cell2.hasPoint = true;
		}
	}
}

std::vector<Vector2>& PoissonDisc::getPoints2D()
{
	return points2D;
}

GridIndex2D PoissonDisc::getGridIndex2D(const Vector2& point)
{
	GridIndex2D gridIndex;

	gridIndex.x = size_t(point.x / grid2D.cellSize);
	gridIndex.y = size_t(point.y / grid2D.cellSize);

	return gridIndex;
}

Cell2D& PoissonDisc::getGridCell2D(const Vector2& point)
{
	GridIndex2D gridIndex = getGridIndex2D(point);

	return grid2D.grid[gridIndex.y][gridIndex.x];
}

Vector2 PoissonDisc::getNextActivePoint2D()
{
	std::uniform_int_distribution<size_t> random(0, activePoints2D.size() - 1);
	size_t index = random(mt);
	Vector2 point = activePoints2D[index];
	activePoints2D.erase(activePoints2D.begin() + index);

	return point;
}

Vector2 PoissonDisc::generateNewPoint2D(const Vector2& origin, double minDistance)
{
	std::uniform_real_distribution<double> random(0.0, 1.0);

	double radius = minDistance * (1.0 + random(mt));
	double angle = 2.0 * M_PI * random(mt);

	Vector2 point;
	point.x = origin.x + radius * cos(angle);
	point.y = origin.y + radius * sin(angle);

	return point;
}

bool PoissonDisc::isInNeighbourhood2D(const Vector2& point, double minDistance)
{
	GridIndex2D gridIndex = getGridIndex2D(point);

	size_t minx = std::max(size_t(0), gridIndex.x - 2);
	size_t maxx = std::min(grid2D.width - 1, gridIndex.x + 2);
	size_t miny = std::max(size_t(0), gridIndex.y - 2);
	size_t maxy = std::min(grid2D.height - 1, gridIndex.y + 2);

	double minDistance2 = minDistance * minDistance;

	for (size_t y = miny; y <= maxy; ++y)
	{
		for (size_t x = minx; x <= maxx; ++x)
		{
			Cell2D& cell = grid2D.grid[y][x];

			if (!cell.hasPoint)
				continue;

			Vector2 pointToNeighbour = cell.point - point;

			if (pointToNeighbour.lengthSquared() < minDistance2)
				return true;
		}
	}

	return false;
}
