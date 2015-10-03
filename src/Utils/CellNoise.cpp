// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Utils/CellNoise.h"
#include "Math/Vector3.h"

using namespace Raycer;

CellNoise::CellNoise()
{
	std::random_device rd;
	seed(rd());
}

CellNoise::CellNoise(uint seed_)
{
	seed(seed_);
}

void CellNoise::seed(uint seed)
{
	m_seed = seed;
}

double CellNoise::getNoise(CellNoiseDistanceType distanceType, CellNoiseCombineType combineType, uint density, double x, double y, double z) const
{
	std::poisson_distribution<uint> poissonDist(density);
	std::uniform_real_distribution<double> realDist(0.0, 1.0);
	std::minstd_rand gen;

	int ix = int(floor(x));
	int iy = int(floor(y));
	int iz = int(floor(z));

	Vector3 evaluationPoint(x, y, z);
	std::array<double, CELL_NOISE_MAX_DISTANCES_COUNT> distances;
	auto it = distances.begin();

	for (int dz = -1; dz <= 1; ++dz)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			for (int dx = -1; dx <= 1; ++dx)
			{
				int cx = ix + dx;
				int cy = iy + dy;
				int cz = iz + dz;

				gen.seed(getHashcode(cx, cy, cz));

				uint pointCount = poissonDist(gen);

				for (uint i = 0; i < pointCount; ++i)
				{
					Vector3 cubePoint;

					cubePoint.x = cx + realDist(gen);
					cubePoint.y = cy + realDist(gen);
					cubePoint.z = cz + realDist(gen);

					if (it != distances.end())
						*it++ = getDistance(distanceType, evaluationPoint, cubePoint);
				}
			}
		}
	}

	std::sort(distances.begin(), it);
	size_t size = it - distances.begin();

	double d1 = 0.0;
	double d2 = 0.0;

	if (size >= 1)
		d1 = distances[0];

	if (size >= 2)
		d2 = distances[1];

	double d = getCombinedValue(combineType, d1, d2);
	return std::abs(d);
}

double CellNoise::getNoise2D(CellNoiseDistanceType distanceType, CellNoiseCombineType combineType, uint density, double x, double y) const
{
	std::poisson_distribution<uint> poissonDist(density);
	std::uniform_real_distribution<double> realDist(0.0, 1.0);
	std::minstd_rand gen;

	int ix = int(floor(x));
	int iy = int(floor(y));

	Vector3 evaluationPoint(x, y, 0.0);
	std::array<double, CELL_NOISE_MAX_DISTANCES_COUNT> distances;
	auto it = distances.begin();

	for (int dy = -1; dy <= 1; ++dy)
	{
		for (int dx = -1; dx <= 1; ++dx)
		{
			int cx = ix + dx;
			int cy = iy + dy;

			gen.seed(getHashcode(cx, cy, 0));

			uint pointCount = poissonDist(gen);

			for (uint i = 0; i < pointCount; ++i)
			{
				Vector3 cubePoint;

				cubePoint.x = cx + realDist(gen);
				cubePoint.y = cy + realDist(gen);

				if (it != distances.end())
					*it++ = getDistance(distanceType, evaluationPoint, cubePoint);
			}
		}
	}

	std::sort(distances.begin(), it);
	size_t size = it - distances.begin();

	double d1 = 0.0;
	double d2 = 0.0;

	if (size >= 1)
		d1 = distances[0];

	if (size >= 2)
		d2 = distances[1];

	double d = getCombinedValue(combineType, d1, d2) / 2.7;
	return std::abs(d);
}

void CellNoise::setVoronoiColors(const std::vector<Color>& colors)
{
	voronoiColors = colors;
}

Color CellNoise::getVoronoiColor(CellNoiseDistanceType distanceType, uint density, double x, double y, double z) const
{
	std::poisson_distribution<uint> poissonDist(density);
	std::uniform_real_distribution<double> realDist(0.0, 1.0);
	std::uniform_int_distribution<size_t> intDist(0, voronoiColors.size() - 1);
	std::minstd_rand gen;

	int ix = int(floor(x));
	int iy = int(floor(y));
	int iz = int(floor(z));

	Vector3 evaluationPoint(x, y, z);
	double minDistance = std::numeric_limits<double>::max();
	size_t closestColorIndex = std::numeric_limits<size_t>::max();

	for (int dz = -1; dz <= 1; ++dz)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			for (int dx = -1; dx <= 1; ++dx)
			{
				int cx = ix + dx;
				int cy = iy + dy;
				int cz = iz + dz;

				gen.seed(getHashcode(cx, cy, cz));

				uint pointCount = poissonDist(gen);

				for (uint i = 0; i < pointCount; ++i)
				{
					Vector3 cubePoint;

					cubePoint.x = cx + realDist(gen);
					cubePoint.y = cy + realDist(gen);
					cubePoint.z = cz + realDist(gen);
					size_t colorIndex = intDist(gen);

					double distance = getDistance(distanceType, evaluationPoint, cubePoint);

					if (distance < minDistance)
					{
						minDistance = distance;
						closestColorIndex = colorIndex;
					}
				}
			}
		}
	}

	Color closestColor;

	if (closestColorIndex < voronoiColors.size())
		closestColor = voronoiColors[closestColorIndex];

	return closestColor;
}

Color CellNoise::getVoronoiColor2D(CellNoiseDistanceType distanceType, uint density, double x, double y) const
{
	std::poisson_distribution<uint> poissonDist(density);
	std::uniform_real_distribution<double> realDist(0.0, 1.0);
	std::uniform_int_distribution<size_t> intDist(0, voronoiColors.size() - 1);
	std::minstd_rand gen;

	int ix = int(floor(x));
	int iy = int(floor(y));

	Vector3 evaluationPoint(x, y, 0.0);
	double minDistance = std::numeric_limits<double>::max();
	size_t closestColorIndex = std::numeric_limits<size_t>::max();

	for (int dy = -1; dy <= 1; ++dy)
	{
		for (int dx = -1; dx <= 1; ++dx)
		{
			int cx = ix + dx;
			int cy = iy + dy;

			gen.seed(getHashcode(cx, cy, 0));

			uint pointCount = poissonDist(gen);

			for (uint i = 0; i < pointCount; ++i)
			{
				Vector3 cubePoint;

				cubePoint.x = cx + realDist(gen);
				cubePoint.y = cy + realDist(gen);
				size_t colorIndex = intDist(gen);

				double distance = getDistance(distanceType, evaluationPoint, cubePoint);

				if (distance < minDistance)
				{
					minDistance = distance;
					closestColorIndex = colorIndex;
				}
			}
		}
	}

	Color closestColor;

	if (closestColorIndex < voronoiColors.size())
		closestColor = voronoiColors[closestColorIndex];

	return closestColor;
}

int CellNoise::getHashcode(int x, int y, int z) const
{
	return (m_seed * 16381) + (x * 17389) + (y * 18313) + (z * 19423);
}

double CellNoise::getDistance(CellNoiseDistanceType distanceType, const Vector3& v1, const Vector3& v2) const
{
	switch (distanceType)
	{
		case CellNoiseDistanceType::EUCLIDEAN:
		{
			return (v1 - v2).length();
		}

		case CellNoiseDistanceType::EUCLIDEAN_SQUARED:
		{
			return (v1 - v2).lengthSquared();
		}

		case CellNoiseDistanceType::MANHATTAN:
		{
			Vector3 diff = Vector3::abs(v1 - v2);

			return diff.x + diff.y + diff.z;
		}

		case CellNoiseDistanceType::CHEBYSHEV:
		{
			Vector3 diff = Vector3::abs(v1 - v2);

			if (diff.x > diff.y && diff.x > diff.z)
				return diff.x;
			else if (diff.y > diff.x && diff.y > diff.z)
				return diff.y;
			else
				return diff.z;
		}
	}

	return (v1 - v2).length();
}

double CellNoise::getCombinedValue(CellNoiseCombineType combineType, double d1, double d2) const
{
	switch (combineType)
	{
		case CellNoiseCombineType::D1: return d1;
		case CellNoiseCombineType::D2: return d2;
		case CellNoiseCombineType::D1_PLUS_D2: return d1 + d2;
		case CellNoiseCombineType::D1_MINUS_D2: return d1 - d2;
		case CellNoiseCombineType::D1_TIMES_D2: return d1 * d2;
		case CellNoiseCombineType::D2_MINUS_D1: return d2 - d1;
	}

	return d1;
}
