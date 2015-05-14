// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <random>
#include <algorithm>
#include <functional>

#include "Utils/CellNoise.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

using namespace Raycer;

CellNoise::CellNoise()
{
	std::random_device rd;
	seed(rd());
}

CellNoise::CellNoise(int seed_)
{
	seed(seed_);
}

void CellNoise::seed(int seed)
{
	m_seed = seed;
}

double CellNoise::getNoise(CellNoiseDistanceType distanceType, CellNoiseCombineType combineType, int density, double x, double y, double z) const
{
	std::vector<double> distances;
	Vector3 evaluationPoint(x, y, z);

	int ix = (int)floor(x);
	int iy = (int)floor(y);
	int iz = (int)floor(z);

	std::poisson_distribution<int> randomPoisson(density);
	std::uniform_real_distribution<double> randomUniform(0.0, 1.0);
	std::mt19937 mt;

	std::function<double(const Vector3& v1, const Vector3& v2)> distanceFunction;

	switch (distanceType)
	{
		case CellNoiseDistanceType::EUCLIDEAN: distanceFunction = &euclideanDistance; break;
		case CellNoiseDistanceType::EUCLIDEAN_SQUARED: distanceFunction = &euclideanDistanceSquared; break;
		case CellNoiseDistanceType::MANHATTAN: distanceFunction = &manhattanDistance; break;
		case CellNoiseDistanceType::CHEBYSHEV: distanceFunction = &chebyshevDistance; break;
		default: distanceFunction = &euclideanDistance; break;
	}

	for (int dz = -1; dz <= 1; ++dz)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			for (int dx = -1; dx <= 1; ++dx)
			{
				int cx = ix + dx;
				int cy = iy + dy;
				int cz = iz + dz;

				mt.seed(getHashcode(cx, cy, cz));

				int pointCount = randomPoisson(mt);

				for (int i = 0; i < pointCount; ++i)
				{
					Vector3 cubePoint;

					cubePoint.x = (double)cx + randomUniform(mt);
					cubePoint.y = (double)cy + randomUniform(mt);
					cubePoint.z = (double)cz + randomUniform(mt);

					distances.push_back(distanceFunction(evaluationPoint, cubePoint));
				}
			}
		}
	}

	std::sort(distances.begin(), distances.end());
	double distance = 0.0;

	switch (combineType)
	{
		case CellNoiseCombineType::D1:
		{
			if (distances.size() >= 1)
				distance = distances[0];

		} break;

		case CellNoiseCombineType::D2:
		{
			if (distances.size() >= 2)
				distance = distances[1];

		} break;

		case CellNoiseCombineType::D1_PLUS_D2:
		{
			if (distances.size() >= 2)
				distance = distances[0] + distances[1];

		} break;

		case CellNoiseCombineType::D1_MINUS_D2:
		{
			if (distances.size() >= 2)
				distance = distances[0] - distances[1];

		} break;

		case CellNoiseCombineType::D2_MINUS_D1:
		{
			if (distances.size() >= 2)
				distance = distances[1] - distances[0];

		} break;

		default: break;
	}

	return fabs(distance);
}

int CellNoise::getHashcode(int x, int y, int z) const
{
	return (m_seed * 16381) + (x * 17389) + (y * 18313) + (z * 19423);
}

double CellNoise::euclideanDistance(const Vector3& v1, const Vector3& v2)
{
	return (v1 - v2).length();
}

double CellNoise::euclideanDistanceSquared(const Vector3& v1, const Vector3& v2)
{
	return (v1 - v2).lengthSquared();
}

double CellNoise::manhattanDistance(const Vector3& v1, const Vector3& v2)
{
	Vector3 diff = Vector3::abs(v1 - v2);

	return diff.x + diff.y + diff.z;
}

double CellNoise::chebyshevDistance(const Vector3& v1, const Vector3& v2)
{
	Vector3 diff = Vector3::abs(v1 - v2);

	if (diff.x > diff.y && diff.x > diff.z)
		return diff.x;
	else if (diff.y > diff.x && diff.y > diff.z)
		return diff.y;
	else
		return diff.z;
}
