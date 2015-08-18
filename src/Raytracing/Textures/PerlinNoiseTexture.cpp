// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Textures/PerlinNoiseTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void PerlinNoiseTexture::initialize()
{
	perlinNoise.seed(seed);
}

Color PerlinNoiseTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	double n = getValue(texcoord, position);

	return baseColor * n;
}

double PerlinNoiseTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)texcoord;

	if (isFbm)
		return perlinNoise.getFbmNoise(octaves, lacunarity, persistence, position.x * scale.x, position.y * scale.y, position.z * scale.z);
	else
		return perlinNoise.getNoise(position.x * scale.x, position.y * scale.y, position.z * scale.z);
}

Vector3 PerlinNoiseTexture::getNormal(const Vector2& texcoord, const Vector3& position, TextureNormalType& type) const
{
	const double epsilon = 0.1;
	double f = getValue(texcoord, position);
	double fx = getValue(texcoord, Vector3(position.x + epsilon, position.y, position.z));
	double fy = getValue(texcoord, Vector3(position.x, position.y + epsilon, position.z));
	double fz = getValue(texcoord, Vector3(position.x, position.y, position.z + epsilon));
	double dfx = (fx - f) / epsilon;
	double dfy = (fy - f) / epsilon;
	double dfz = (fz - f) / epsilon;

	type = TextureNormalType::GRADIENT;

	return Vector3(dfx, dfy, dfz);
}
