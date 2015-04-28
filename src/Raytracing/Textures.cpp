// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <algorithm>

#include "Raytracing/Textures.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

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

double ColorTexture::getValue(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return 1.0;
}

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

void ImageTexture::initialize()
{
	image.load(imageFilePath);
}

Color ImageTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;

	return image.getPixelLinear(texcoord.x, texcoord.y);
}

double ImageTexture::getValue(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return 1.0;
}

void PerlinNoiseTexture::initialize()
{
}

Color PerlinNoiseTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return Color();
}

double PerlinNoiseTexture::getValue(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return 1.0;
}

void CellNoiseTexture::initialize()
{
}

Color CellNoiseTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return Color();
}

double CellNoiseTexture::getValue(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return 1.0;
}

void MarbleTexture::initialize()
{
}

Color MarbleTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)texcoord;

	double n1 = fabs(cos(position.x * 10.0 + perlinNoise.getFbmNoise(8, 2.0, 0.5, position.x * 2.0, position.y * 2.0, position.z * 2.0) * 15.0));
	n1 = (1.0 - n1) / 2.0;

	Color marbleWhite(255, 252, 240);
	Color oxfordBlue(0, 33, 71);
	oxfordBlue.a = n1;

	return Color::alphaBlend(marbleWhite, oxfordBlue);
}

double MarbleTexture::getValue(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return 1.0;
}

void WoodTexture::initialize()
{
}

Color WoodTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)texcoord;

	// large scale pattern with sharp edges
	double n1 = perlinNoise.getFbmNoise(8, 2.0, 0.3, position.x * 2.0, position.y * 0.1, position.z * 2.0);
	n1 *= 8.0;
	n1 -= (int)n1;
	n1 += 0.4;
	n1 = std::min(n1, 1.0);

	// subtle bumpiness
	double n2 = 0.8 + perlinNoise.getNoise(position.x * 16.0, position.y * 16.0, position.z * 16.0) / 5.0;

	// subtle streaks
	double n3 = perlinNoise.getFbmNoise(4, 2.0, 0.01, position.x * 200.0, position.y * 1.0, position.z * 2.0);
	n3 = 1.0 - n3 * n3 * n3 * n3;
	n3 = 0.75 + n3 / 4.0;

	return Color(110, 71, 11) * (n1 * n2 * n3);
}

double WoodTexture::getValue(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return 1.0;
}

void FireTexture::initialize()
{
}

Color FireTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return Color();
}

double FireTexture::getValue(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return 1.0;
}
