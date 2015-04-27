// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>

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

double CellNoise::getNoise2D(CellNoiseType type, int order, int density, double x, double y) const
{
	std::vector<double> distances;
	Vector2 evaluationPoint(x, y);

	int ix = (int)floor(x);
	int iy = (int)floor(y);

	std::poisson_distribution<int> randomPoisson(density);
	std::uniform_real_distribution<double> randomUniform(0.0, 1.0);
	std::mt19937 mt;

	std::function<double(const Vector2& v1, const Vector2& v2)> distanceFunction;

	switch (type)
	{
		case CellNoiseType::EUCLIDEAN: distanceFunction = &euclideanDistance2D; break;
		case CellNoiseType::EUCLIDEAN_SQUARED: distanceFunction = &euclideanDistanceSquared2D; break;
		case CellNoiseType::MANHATTAN: distanceFunction = &manhattanDistance2D; break;
		case CellNoiseType::CHEBYSHEV: distanceFunction = &chebyshevDistance2D; break;
		default: distanceFunction = &euclideanDistance2D; break;
	}

	for (int dy = -1; dy <= 1; ++dy)
	{
		for (int dx = -1; dx <= 1; ++dx)
		{
			int cx = ix + dx;
			int cy = iy + dy;

			mt.seed(getHashcode2D(cx, cy));

			int pointCount = randomPoisson(mt);

			for (int i = 0; i < pointCount; ++i)
			{
				Vector2 newPoint;

				newPoint.x = (double)cx + randomUniform(mt);
				newPoint.y = (double)cy + randomUniform(mt);

				distances.push_back(distanceFunction(evaluationPoint, newPoint));
			}
		}
	}

	std::sort(distances.begin(), distances.end());

	double distance = 0.0;

	if (order < distances.size())
		distance = distances[order];

	return std::min(distance, 1.0);
}

double CellNoise::getNoise3D(CellNoiseType type, int order, int density, double x, double y, double z) const
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

	switch (type)
	{
		case CellNoiseType::EUCLIDEAN: distanceFunction = &euclideanDistance3D; break;
		case CellNoiseType::EUCLIDEAN_SQUARED: distanceFunction = &euclideanDistanceSquared3D; break;
		case CellNoiseType::MANHATTAN: distanceFunction = &manhattanDistance3D; break;
		case CellNoiseType::CHEBYSHEV: distanceFunction = &chebyshevDistance3D; break;
		default: distanceFunction = &euclideanDistance3D; break;
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

				mt.seed(getHashcode3D(cx, cy, cz));

				int pointCount = randomPoisson(mt);

				for (int i = 0; i < pointCount; ++i)
				{
					Vector3 newPoint;

					newPoint.x = (double)cx + randomUniform(mt);
					newPoint.y = (double)cy + randomUniform(mt);
					newPoint.z = (double)cz + randomUniform(mt);

					distances.push_back(distanceFunction(evaluationPoint, newPoint));
				}
			}
		}
	}

	std::sort(distances.begin(), distances.end());

	double distance = 0.0;

	if (order < distances.size())
		distance = distances[order];

	return std::min(distance, 1.0);
}

double CellNoise::getFbmNoise2D(CellNoiseType type, int order, int density, int octaves, double lacunarity, double persistence, double x, double y) const
{
	double result = 0.0;
	double frequency = 1.0;
	double amplitude = 1.0;

	for (int i = 0; i < octaves; ++i)
	{
		result += getNoise2D(type, order, density, x * frequency, y * frequency) * amplitude;
		frequency *= lacunarity;
		amplitude *= persistence;
	}

	return result;
}

double CellNoise::getFbmNoise3D(CellNoiseType type, int order, int density, int octaves, double lacunarity, double persistence, double x, double y, double z) const
{
	double result = 0.0;
	double frequency = 1.0;
	double amplitude = 1.0;

	for (int i = 0; i < octaves; ++i)
	{
		result += getNoise3D(type, order, density, x * frequency, y * frequency, z * frequency) * amplitude;
		frequency *= lacunarity;
		amplitude *= persistence;
	}

	return result;
}

int CellNoise::getHashcode2D(int x, int y) const
{
	return (m_seed * 16381) + (x * 17389) + (y * 18313);
}

int CellNoise::getHashcode3D(int x, int y, int z) const
{
	return (m_seed * 16381) + (x * 17389) + (y * 18313) + (z * 19423);
}

double CellNoise::euclideanDistance2D(const Vector2& v1, const Vector2& v2)
{
	return (v1 - v2).length();
}

double CellNoise::euclideanDistance3D(const Vector3& v1, const Vector3& v2)
{
	return (v1 - v2).length();
}

double CellNoise::euclideanDistanceSquared2D(const Vector2& v1, const Vector2& v2)
{
	return (v1 - v2).lengthSquared();
}

double CellNoise::euclideanDistanceSquared3D(const Vector3& v1, const Vector3& v2)
{
	return (v1 - v2).lengthSquared();
}

double CellNoise::manhattanDistance2D(const Vector2& v1, const Vector2& v2)
{
	Vector2 diff = Vector2::abs(v1 - v2);

	return diff.x + diff.y;
}

double CellNoise::manhattanDistance3D(const Vector3& v1, const Vector3& v2)
{
	Vector3 diff = Vector3::abs(v1 - v2);

	return diff.x + diff.y + diff.z;
}

double CellNoise::chebyshevDistance2D(const Vector2& v1, const Vector2& v2)
{
	Vector2 diff = Vector2::abs(v1 - v2);

	if (diff.x > diff.y)
		return diff.x;
	else
		return diff.y;
}

double CellNoise::chebyshevDistance3D(const Vector3& v1, const Vector3& v2)
{
	Vector3 diff = Vector3::abs(v1 - v2);

	if (diff.x > diff.y && diff.x > diff.z)
		return diff.x;
	else if (diff.y > diff.x && diff.y > diff.z)
		return diff.y;
	else
		return diff.z;
}
