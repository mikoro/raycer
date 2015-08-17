// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Textures/PerlinNoiseTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void PerlinNoiseTexture::initialize()
{
	perlinNoise.seed(seed);
}

Color PerlinNoiseTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	double n = getValue(texcoord, position);

	return baseColor * n;
}

double PerlinNoiseTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;

	if (isFbm)
		return perlinNoise.getFbmNoise(octaves, lacunarity, persistence, position.x * scale.x, position.y * scale.y, position.z * scale.z);
	else
		return perlinNoise.getNoise(position.x * scale.x, position.y * scale.y, position.z * scale.z);
}
