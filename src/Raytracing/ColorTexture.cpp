// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/ColorTexture.h"
#include "Math/Color.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"

using namespace Raycer;

void ColorTexture::initialize()
{
}

Color ColorTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return color;
}
