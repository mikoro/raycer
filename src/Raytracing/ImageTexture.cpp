// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/ImageTexture.h"
#include "Math/Color.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"

using namespace Raycer;

void ImageTexture::initialize()
{
	image.load(imageFilePath);
}

Color ImageTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;

	return image.getPixelLinear(texcoord.x, texcoord.y);
}
