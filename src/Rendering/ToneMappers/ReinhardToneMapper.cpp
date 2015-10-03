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
		logSum += log(epsilon + pixelData[size_t(i)].getLuminance());

	double logAvgLuminance = exp(logSum / pixelCount);
	double scale = scene.toneMapper.key / logAvgLuminance;
	double maxLuminance2 = scene.toneMapper.maxLuminance * scene.toneMapper.maxLuminance;
	double invGamma = 1.0 / scene.toneMapper.gamma;

	#pragma omp parallel for
	for (int i = 0; i < pixelCount; ++i)
	{
		// fix static analysis warnings
		size_t j = size_t(i);

		double originalLuminance = pixelData[j].getLuminance();
		double scaledLuminance = scale * originalLuminance;
		double mappedLuminance = (scaledLuminance * (1.0 + (scaledLuminance / maxLuminance2))) / (1.0 + scaledLuminance);
		double colorScale = mappedLuminance / originalLuminance;

		pixelData[j] *= colorScale;
		pixelData[j].a = 1.0;

		if (scene.toneMapper.applyGamma)
			pixelData[j] = Color::pow(pixelData[j], invGamma);

		if (scene.toneMapper.shouldClamp)
			pixelData[j].clamp();
	}
}
