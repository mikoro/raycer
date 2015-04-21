// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/ImageTexture.h"
#include "Math/Color.h"

using namespace Raycer;

void ImageTexture::initialize()
{
	image.load(imageFilePath);
	image.swapBytes();
}

Color ImageTexture::getColor(double u, double v) const
{
	return image.getPixel(u, v);
}
