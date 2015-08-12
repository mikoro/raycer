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
}

Color ImageTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;

	return image.getPixelBilinear(texcoord.x, texcoord.y);
}

double ImageTexture::getValue(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return 1.0;
}
