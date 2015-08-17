// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Textures/ImageTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void ImageTexture::initialize()
{
	image.load(imageFilePath);

	if (applyGamma)
		image.applyGamma(gamma);
}

Color ImageTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	(void)position;

	return image.getPixelBilinear(texcoord.x, texcoord.y);
}

double ImageTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)position;

	Color color = image.getPixelBilinear(texcoord.x, texcoord.y);

	return 0.2126 * pow(color.r, gamma) + 0.7152 * pow(color.g, gamma) + 0.0722 * pow(color.b, gamma);
}

Vector2 ImageTexture::get2DValue(const Vector2& texcoord, const Vector3& position) const
{
	double value = getValue(texcoord, position);

	return Vector2(value, value);
}
