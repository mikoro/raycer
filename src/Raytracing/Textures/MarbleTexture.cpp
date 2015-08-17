// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Raytracing/Textures/MarbleTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void MarbleTexture::initialize()
{
	perlinNoise.seed(seed);
}

Color MarbleTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;

	double n1 = fabs(cos(position.x * density + perlinNoise.getFbmNoise(8, 2.0, 0.5, position.x * 2.0, position.y * 2.0, position.z * 2.0) * swirliness));
	n1 = (1.0 - n1) / transparency;

	Color streakColor1(streakColor);
	streakColor1.a = n1;

	return Color::alphaBlend(marbleColor, streakColor1);
}

double MarbleTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)position;
	(void)texcoord;

	return 1.0;
}
