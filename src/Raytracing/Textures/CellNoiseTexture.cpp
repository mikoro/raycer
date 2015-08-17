// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Textures/CellNoiseTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void CellNoiseTexture::initialize()
{
	cellNoise.seed(seed);
}

Color CellNoiseTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	double n = getValue(texcoord, position);

	return baseColor * n;
}

double CellNoiseTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;

	return cellNoise.getNoise(distanceType, combineType, density, position.x * scale.x, position.y * scale.y, position.z * scale.z);
}

Vector2 CellNoiseTexture::get2DValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;
	(void)position;

	return Vector2(0.0, 0.0);
}
