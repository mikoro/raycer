// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Textures/FireTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void FireTexture::initialize()
{
	perlinNoise.seed(seed);
}

Color FireTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	double n1 = perlinNoise.getFbmNoise(8, 2.0, 0.5, position.x * 5.0, position.y * 5.0, position.z * 5.0);
	//n1 = n1 * n1 * n1;

	if (n1 < 0.9)
		n1 /= 5.0;

	return Color(n1, n1, n1);
}

double FireTexture::getValue(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return 1.0;
}
