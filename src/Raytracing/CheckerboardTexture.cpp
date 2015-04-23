// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/CheckerboardTexture.h"
#include "Math/Color.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"

using namespace Raycer;

void CheckerboardTexture::initialize()
{
}

Color CheckerboardTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;

	if (texcoord.x < 0.5)
		return (texcoord.y < 0.5) ? color1 : color2;
	else
		return (texcoord.y < 0.5) ? color2 : color1;
}
