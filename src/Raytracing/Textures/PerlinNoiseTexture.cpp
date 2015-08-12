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

Color PerlinNoiseTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	double n = getValue(position, texcoord);

	return baseColor * n;
}

double PerlinNoiseTexture::getValue(const Vector3& position, const Vector2& texcoord) const
{
	(void)texcoord;

	if (isFbm)
		return perlinNoise.getFbmNoise(octaves, lacunarity, persistence, position.x * scale.x, position.y * scale.y, position.z * scale.z);
	else
		return perlinNoise.getNoise(position.x * scale.x, position.y * scale.y, position.z * scale.z);
}
