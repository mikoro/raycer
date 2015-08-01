// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Textures/CheckerTexture.h"
#include "Math/Vector2.h"
#include "Math/Color.h"

using namespace Raycer;

void CheckerTexture::initialize()
{
}

Color CheckerTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;

	if (texcoord.x < 0.5)
		return (texcoord.y < 0.5) ? color1 : color2;
	else
		return (texcoord.y < 0.5) ? color2 : color1;
}

double CheckerTexture::getValue(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return 1.0;
}
