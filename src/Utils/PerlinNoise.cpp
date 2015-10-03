// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Utils/PerlinNoise.h"

using namespace Raycer;

PerlinNoise::PerlinNoise()
{
	std::random_device rd;
	seed(rd());
}

PerlinNoise::PerlinNoise(uint seed_)
{
	seed(seed_);
}

void PerlinNoise::seed(uint seed)
{
	permutations.clear();
	permutations.resize(256);
	std::iota(permutations.begin(), permutations.end(), 0);
	std::mt19937 mt(seed);
	std::shuffle(permutations.begin(), permutations.end(), mt);
	std::vector<size_t> duplicate = permutations;
	permutations.insert(permutations.end(), duplicate.begin(), duplicate.end());
}

double PerlinNoise::getNoise(double x, double y, double z) const
{
	size_t X = size_t(floor(x)) & 255;
	size_t Y = size_t(floor(y)) & 255;
	size_t Z = size_t(floor(z)) & 255;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	double u = fade(x);
	double v = fade(y);
	double w = fade(z);

	size_t A = permutations[X] + Y;
	size_t AA = permutations[A] + Z;
	size_t AB = permutations[A + 1] + Z;
	size_t B = permutations[X + 1] + Y;
	size_t BA = permutations[B] + Z;
	size_t BB = permutations[B + 1] + Z;

	double n = lerp(w, lerp(v, lerp(u, grad(permutations[AA], x, y, z),
		grad(permutations[BA], x - 1, y, z)),
		lerp(u, grad(permutations[AB], x, y - 1, z),
		grad(permutations[BB], x - 1, y - 1, z))),
		lerp(v, lerp(u, grad(permutations[AA + 1], x, y, z - 1),
		grad(permutations[BA + 1], x - 1, y, z - 1)),
		lerp(u, grad(permutations[AB + 1], x, y - 1, z - 1),
		grad(permutations[BB + 1], x - 1, y - 1, z - 1))));

	return std::max(0.0, std::min(0.5 + n / 2.0, 1.0)); // move and clamp to 0.0-1.0 range
}

double PerlinNoise::getFbmNoise(uint octaves, double lacunarity, double persistence, double x, double y, double z) const
{
	double result = 0.0;
	double frequency = 1.0;
	double amplitude = 1.0;

	for (uint i = 0; i < octaves; ++i)
	{
		result += getNoise(x * frequency, y * frequency, z * frequency) * amplitude;
		frequency *= lacunarity;
		amplitude *= persistence;
	}

	return result;
}

double PerlinNoise::fade(double t) const
{
	return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

double PerlinNoise::lerp(double t, double a, double b) const
{
	return a + t * (b - a);
}

double PerlinNoise::grad(size_t hash, double x, double y, double z) const
{
	size_t h = hash & 15;
	double u = (h < 8) ? x : y;
	double v = (h < 4) ? y : ((h == 12 || h == 14) ? x : z);

	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
