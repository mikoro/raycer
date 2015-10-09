// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/ToneMappers/ReinhardToneMapper.h"
#include "Raytracing/Scene.h"
#include "Rendering/Image.h"
#include "Math/Color.h"

using namespace Raycer;

void ReinhardToneMapper::apply(const Scene& scene, const Image& inputImage, Image& outputImage)
{
	const std::vector<Color>& inputPixelData = inputImage.getPixelDataConst();
	std::vector<Color>& outputPixelData = outputImage.getPixelData();

	double epsilon = std::numeric_limits<double>::epsilon();
	int pixelCount = int(inputPixelData.size());
	double logSum = 0.0;

	#pragma omp parallel for reduction(+:logSum)
	for (int i = 0; i < pixelCount; ++i)
		logSum += log(epsilon + inputPixelData.at(size_t(i)).getLuminance());

	double logAvgLuminance = exp(logSum / pixelCount);
	double scale = scene.toneMapper.key / logAvgLuminance;
	double maxLuminance2 = scene.toneMapper.maxLuminance * scene.toneMapper.maxLuminance;
	double invGamma = 1.0 / scene.toneMapper.gamma;

	#pragma omp parallel for
	for (int i = 0; i < pixelCount; ++i)
	{
		// fix static analysis warnings
		size_t j = size_t(i);

		double originalLuminance = inputPixelData.at(j).getLuminance();
		double scaledLuminance = scale * originalLuminance;
		double mappedLuminance = (scaledLuminance * (1.0 + (scaledLuminance / maxLuminance2))) / (1.0 + scaledLuminance);
		double colorScale = mappedLuminance / originalLuminance;

		outputPixelData[j] = inputPixelData.at(j) * colorScale;
		outputPixelData[j].a = 1.0;

		if (scene.toneMapper.applyGamma)
			outputPixelData[j] = Color::pow(outputPixelData[j], invGamma);

		if (scene.toneMapper.shouldClamp)
			outputPixelData[j].clamp();
	}
}
