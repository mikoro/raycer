// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/ToneMappers/ReinhardToneMapper.h"
#include "Raytracing/Scene.h"
#include "Rendering/Image.h"
#include "Math/Color.h"

using namespace Raycer;

void ReinhardToneMapper::apply(const Scene& scene, Image& image)
{
	std::vector<Color>& pixelData = image.getPixelData();

	double epsilon = std::numeric_limits<double>::epsilon();
	int pixelCount = int(pixelData.size());
	double logSum = 0.0;

	#pragma omp parallel for reduction(+:logSum)
	for (int i = 0; i < pixelCount; ++i)
		logSum += log(epsilon + pixelData[i].getLuminance());

	double logAvgLuminance = exp(logSum / pixelCount);
	double scale = scene.toneMapper.key / logAvgLuminance;
	double maxLuminance2 = scene.toneMapper.maxLuminance * scene.toneMapper.maxLuminance;
	double invGamma = 1.0 / scene.toneMapper.gamma;

	#pragma omp parallel for
	for (int i = 0; i < pixelCount; ++i)
	{
		double originalLuminance = pixelData[i].getLuminance();
		double scaledLuminance = scale * originalLuminance;
		double mappedLuminance = (scaledLuminance * (1.0 + (scaledLuminance / maxLuminance2))) / (1.0 + scaledLuminance);
		double colorScale = mappedLuminance / originalLuminance;

		pixelData[i] *= colorScale;
		pixelData[i].a = 1.0;

		if (scene.toneMapper.applyGamma)
			pixelData[i] = Color::pow(pixelData[i], invGamma);

		if (scene.toneMapper.shouldClamp)
			pixelData[i].clamp();
	}
}
