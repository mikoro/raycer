// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Textures/VoronoiTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void VoronoiTexture::initialize()
{
	cellNoise.seed(seed);

	if (useRandomColors)
	{
		colors.clear();

		std::mt19937 generator(seed);

		for (uint64_t i = 0; i < randomColorCount; ++i)
			colors.push_back(Color::random(generator));
	}

	cellNoise.setVoronoiColors(colors);
}

Color VoronoiTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;

	return cellNoise.getVoronoiColor(distanceType, density, position.x * scale.x, position.y * scale.y, position.z * scale.z);
}

double VoronoiTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;
	(void)position;

	return 0.0;
}

Vector3 VoronoiTexture::getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const
{
	(void)texcoord;
	(void)position;

	type = TextureNormalDataType::NORMAL_MAP;

	return Vector3(0.0, 0.0, 0.0);
}
