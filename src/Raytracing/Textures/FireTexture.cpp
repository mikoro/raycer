// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Textures/FireTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void FireTexture::initialize()
{
	perlinNoise.seed(seed);

	colorGradient.addSegment(Color(0, 0, 0), Color(0, 0, 0), 50);
	colorGradient.addSegment(Color(0, 0, 0), Color(5, 5, 5), 4);
	colorGradient.addSegment(Color(5, 5, 5), Color(255, 0, 0), 6);
	colorGradient.addSegment(Color(255, 0, 0), Color(255, 255, 0), 30);
	colorGradient.addSegment(Color(255, 255, 0), Color(255, 255, 255), 10);
}

Color FireTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;

	double n1 = perlinNoise.getFbmNoise(8, 2.0, 0.75, position.x * 4.0, position.y * 4.0, position.z * 4.0) / 3.0;
	n1 = std::min(n1, 1.0);

	return colorGradient.getColor(n1);
}

double FireTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;
	(void)position;

	return 0.0;
}

Vector3 FireTexture::getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const
{
	(void)texcoord;
	(void)position;

	type = TextureNormalDataType::NONE;
	return Vector3(0.0, 0.0, 0.0);
}
