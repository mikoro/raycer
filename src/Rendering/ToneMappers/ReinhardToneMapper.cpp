// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Rendering/ToneMappers/ReinhardToneMapper.h"
#include "Rendering/Image.h"
#include "Math/Color.h"

using namespace Raycer;

ReinhardToneMapper::ReinhardToneMapper(bool applyGamma_, double gamma_, bool shouldClamp_, double key_, double maxLuminance_)
{
	applyGamma = applyGamma_;
	gamma = gamma_;
	shouldClamp = shouldClamp_;
	key = key_;
	maxLuminance = maxLuminance_;
}

void ReinhardToneMapper::apply(Image& image)
{
	std::vector<Color>& pixelData = image.getPixelData();

	double epsilon = std::numeric_limits<double>::epsilon();
	int pixelCount = (int)pixelData.size();
	double logSum = 0.0;

	#pragma omp parallel for reduction(+:logSum)
	for (int i = 0; i < pixelCount; ++i)
		logSum += log(epsilon + pixelData[i].getLuminance());

	double logAvgLuminance = exp(logSum / (double)pixelCount);
	double scale = key / logAvgLuminance;
	double maxLuminance2 = maxLuminance * maxLuminance;
	double invGamma = 1.0 / gamma;

	#pragma omp parallel for
	for (int i = 0; i < pixelCount; ++i)
	{
		double originalLuminance = pixelData[i].getLuminance();
		double scaledLuminance = scale * originalLuminance;
		double mappedLuminance = (scaledLuminance * (1.0 + (scaledLuminance / maxLuminance2))) / (1.0 + scaledLuminance);
		double colorScale = mappedLuminance / originalLuminance;

		pixelData[i] *= colorScale;
		pixelData[i].a = 1.0;

		if (applyGamma)
			pixelData[i] = Color::pow(pixelData[i], invGamma);
		
		if (shouldClamp)
			pixelData[i].clamp();
	}
}
