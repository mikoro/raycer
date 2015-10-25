// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <random>
#include <vector>

#include "Math/Vector2.h"

namespace Raycer
{
	struct PoissonDiscCell
	{
		Vector2 point;
		bool hasPoint = false;
	};

	struct PoissonDiscCellIndex
	{
		uint64_t x = 0;
		uint64_t y = 0;
	};

	class PoissonDisc
	{
	public:

		PoissonDisc();
		explicit PoissonDisc(uint32_t seed);

		void seed(uint32_t seed);
		std::vector<Vector2> generate(uint64_t width, uint64_t height, double minDistance, uint64_t iterationLimit = 30, bool normalize = false);

	private:

		PoissonDiscCellIndex getCellIndex(const Vector2& point);
		PoissonDiscCell& getCell(const Vector2& point);
		Vector2 getNextPointToProcess();
		Vector2 generateNewPoint(const Vector2& origin);
		bool isTooCloseToOthers(const Vector2& point);

		std::vector<std::vector<PoissonDiscCell>> grid;
		uint64_t gridWidth = 0;
		uint64_t gridHeight = 0;
		double cellSize = 0.0;
		double minDistance = 0.0;

		std::vector<Vector2> points;
		std::vector<Vector2> pointsToProcess;

		std::mt19937 generator;
	};
}
