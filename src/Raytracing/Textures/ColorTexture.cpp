// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Textures/ColorTexture.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

using namespace Raycer;

void ColorTexture::initialize()
{
}

Color ColorTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;
	(void)position;

	return color;
}

double ColorTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;
	(void)position;

	return 0.0;
}

Vector3 ColorTexture::getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const
{
	(void)texcoord;
	(void)position;

	type = TextureNormalDataType::NONE;
	return Vector3(0.0, 0.0, 0.0);
}
