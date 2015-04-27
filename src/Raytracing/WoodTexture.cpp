// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>

#include "Raytracing/WoodTexture.h"
#include "Math/Color.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"

using namespace Raycer;

WoodTexture::WoodTexture() : perlinNoise(67587)
{
}

void WoodTexture::initialize()
{
}

Color WoodTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)texcoord;

	// large scale pattern with sharp edges
	double n1 = perlinNoise.getFbmNoise(8, 2.0, 0.3, position.x * 2.0, position.y * 0.1, position.z * 2.0);
	n1 *= 8.0;
	n1 -= (int)n1;
	n1 += 0.4;
	n1 = std::min(n1, 1.0);

	// subtle bumpiness
	double n2 = 0.8 + perlinNoise.getNoise(position.x * 16.0, position.y * 16.0, position.z * 16.0) / 5.0;
	
	// subtle streaks
	double n3 = perlinNoise.getFbmNoise(4, 2.0, 0.01, position.x * 200.0, position.y * 1.0, position.z * 2.0);
	n3 = 1.0 - n3 * n3 * n3 * n3;
	n3 = 0.75 + n3 / 4.0;

	return Color(110, 71, 11) * (n1 * n2 * n3);
}
