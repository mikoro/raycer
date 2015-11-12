// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Textures/AtmosphereTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Math/MathUtils.h"

using namespace Raycer;

void AtmosphereTexture::initialize()
{
	gradient.addSegment(topColor1, topColor2, 20);
	gradient.addSegment(topColor2, middleColor, 20);
	gradient.addSegment(middleColor, horizonColor, 10);
	gradient.addSegment(horizonColor, middleColor, 10);
	gradient.addSegment(middleColor, topColor2, 20);
	gradient.addSegment(topColor2, topColor1, 20);

	noise.seed(seed);

	sunDirection = -sunOrientation.getDirection();
}

Color AtmosphereTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	Color skyColor = gradient.getColor(texcoord.y);

	if (hasClouds)
	{
		// fade out clouds near horizon
		double h = std::abs(0.5 - texcoord.y) * 8.0;
		h = std::min(1.0, h);

		double n = noise.getFbmNoise(8, 4.0, 0.25, position.x / 50.0, position.y / 50.0, position.z / 50.0);
		skyColor += (n * h) * cloudColor;
	}

	if (hasSun)
	{
		Vector3 rayDirection = position.normalized();
		double d = rayDirection.dot(sunDirection);

		if (d > 0.0)
		{
			d = MathUtils::fastPow(d, sunSize);
			skyColor += d * sunColor;
		}
	}

	return skyColor;
}

double AtmosphereTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;
	(void)position;

	return 0.0;
}

Vector3 AtmosphereTexture::getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const
{
	(void)texcoord;
	(void)position;

	type = TextureNormalDataType::NONE;
	return Vector3(0.0, 0.0, 0.0);
}
