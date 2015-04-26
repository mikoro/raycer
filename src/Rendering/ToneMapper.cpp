// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Rendering/ToneMapper.h"
#include "Rendering/Image.h"
#include "Math/Color.h"

using namespace Raycer;

void ToneMapper::gamma(Image& image, double gamma)
{
	int length = image.getLength();

	#pragma omp parallel for
	for (int i = 0; i < length; ++i)
	{
		image.pixelData[i] = Color::pow(image.pixelData[i], gamma).clamped();
		image.pixelData[i].a = 1.0;
	}
}
