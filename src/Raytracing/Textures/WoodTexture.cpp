// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Textures/WoodTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void WoodTexture::initialize()
{
	perlinNoise.seed(seed);
}

Color WoodTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;

	// large scale pattern with sharp edges
	double n1 = perlinNoise.getFbmNoise(8, 2.0, 0.3, position.x * 2.0, position.y * 0.1, position.z * 2.0);
	n1 *= density;
	n1 -= (int)n1;
	n1 += 0.4;
	n1 = std::min(n1, 1.0);

	// subtle bumpiness
	double n2 = (1.0 - 1.0 / bumpiness) + perlinNoise.getNoise(position.x * 16.0, position.y * 16.0, position.z * 16.0) / bumpiness;

	// subtle streaks
	double n3 = perlinNoise.getFbmNoise(4, 2.0, 0.01, position.x * 200.0, position.y * 1.0, position.z * 2.0);
	n3 = 1.0 - n3 * n3 * n3 * n3;
	n3 = 0.75 + n3 / 4.0;

	return woodColor * (n1 * n2 * n3);
}

double WoodTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;
	(void)position;

	return 0.0;
}

Vector3 WoodTexture::getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const
{
	(void)texcoord;
	(void)position;

	type = TextureNormalDataType::NORMAL_MAP;

	return Vector3(0.0, 0.0, 0.0);
}
