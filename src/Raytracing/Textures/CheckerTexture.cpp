// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Textures/CheckerTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void CheckerTexture::initialize()
{
}

Color CheckerTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	(void)position;

	if (stripeMode)
	{
		if (texcoord.x < stripeWidth || texcoord.y < stripeWidth || texcoord.x > (1.0 - stripeWidth) || texcoord.y > (1.0 - stripeWidth))
			return color1;
		else
			return color2;
	}
	else
	{
		if (texcoord.x < 0.5)
			return (texcoord.y < 0.5) ? color1 : color2;
		else
			return (texcoord.y < 0.5) ? color2 : color1;
	}
}

double CheckerTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;
	(void)position;

	return 0.0;
}

Vector3 CheckerTexture::getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const
{
	(void)texcoord;
	(void)position;

	type = TextureNormalDataType::NORMAL_MAP;

	return Vector3(0.0, 0.0, 0.0);
}
