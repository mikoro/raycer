// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Rendering/ToneMappers/LinearToneMapper.h"
#include "Rendering/Image.h"
#include "Math/Color.h"

using namespace Raycer;

LinearToneMapper::LinearToneMapper(bool applyGamma_, double gamma_, bool shouldClamp_, double exposure_)
{
	applyGamma = applyGamma_;
	gamma = gamma_;
	shouldClamp = shouldClamp_;
	exposure = exposure_;
}

void LinearToneMapper::apply(Image& image)
{
	std::vector<Color>& pixelData = image.getPixelData();
	double invGamma = 1.0 / gamma;

	#pragma omp parallel for
	for (int i = 0; i < (int)pixelData.size(); ++i)
	{
		pixelData[i] *= pow(2.0, exposure);
		pixelData[i].a = 1.0;

		if (applyGamma)
			pixelData[i] = Color::pow(pixelData[i], invGamma);

		if (shouldClamp)
			pixelData[i].clamp();
	}
}
