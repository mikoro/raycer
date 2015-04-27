// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cmath>

#include "Raytracing/MarbleTexture.h"
#include "Math/Color.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"

using namespace Raycer;

MarbleTexture::MarbleTexture() : perlinNoise(21323)
{
}

void MarbleTexture::initialize()
{
}

Color MarbleTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)texcoord;

	double n1 = fabs(cos(position.x * 10.0 + perlinNoise.getFbmNoise(8, 2.0, 0.5, position.x * 2.0, position.y * 2.0, position.z * 2.0) * 15.0));
	n1 = (1.0 - n1) / 2.0;

	Color marbleWhite(255, 252, 240);
	Color oxfordBlue(0, 33, 71);
	oxfordBlue.a = n1;

	return Color::alphaBlend(marbleWhite, oxfordBlue);
}
