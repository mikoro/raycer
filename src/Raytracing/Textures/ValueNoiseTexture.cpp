// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Textures/ValueNoiseTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void ValueNoiseTexture::initialize()
{
	valueNoise.seed(seed);
}

Color ValueNoiseTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	double n = getValue(texcoord, position);

	return baseColor * n;
}

double ValueNoiseTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;

	if (isFbm)
		return valueNoise.getFbmNoise(octaves, lacunarity, persistence, position.x * scale.x, position.y * scale.y, position.z * scale.z);
	else
		return valueNoise.getNoise(position.x * scale.x, position.y * scale.y, position.z * scale.z);
}
