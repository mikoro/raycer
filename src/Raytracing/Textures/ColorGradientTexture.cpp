// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Textures/ColorGradientTexture.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

using namespace Raycer;

void ColorGradientTexture::initialize()
{
}

Color ColorGradientTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	(void)position;

	Color gradientColor;

	if (hasHorizontalColorGradient)
		gradientColor += horizontalColorGradient.getColor(texcoord.x) * horizontalIntensity;

	if (hasVerticalColorGradient)
		gradientColor += verticalColorGradient.getColor(texcoord.y) * verticalIntensity;

	return gradientColor;
}

double ColorGradientTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;
	(void)position;

	return 0.0;
}

Vector3 ColorGradientTexture::getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const
{
	(void)texcoord;
	(void)position;

	type = TextureNormalDataType::NONE;
	return Vector3(0.0, 0.0, 0.0);
}
