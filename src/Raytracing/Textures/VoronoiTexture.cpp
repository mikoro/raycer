// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

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

		std::mt19937 gen(seed);

		for (int i = 0; i < randomColorCount; ++i)
			colors.push_back(Color::random(gen));
	}

	cellNoise.setVoronoiColors(colors);
}

Color VoronoiTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)texcoord;

	return cellNoise.getVoronoiColor(distanceType, density, position.x * scale.x, position.y * scale.y, position.z * scale.z);
}

double VoronoiTexture::getValue(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return 1.0;
}
