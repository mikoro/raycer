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
	const AlignedColorfVector& inputPixelData = inputImage.getPixelDataConst();
	AlignedColorfVector& outputPixelData = outputImage.getPixelData();

	double epsilon = std::numeric_limits<double>::epsilon();
	int64_t pixelCount = int64_t(inputPixelData.size());
	double logSum = 0.0;

	#pragma omp parallel for reduction(+:logSum)
	for (int64_t i = 0; i < pixelCount; ++i)
		logSum += log(epsilon + inputPixelData.at(size_t(i)).toColor().getLuminance());

	double logAvgLuminance = exp(logSum / double(pixelCount));
	double scale = scene.toneMapper.key / logAvgLuminance;
	double maxLuminance2 = scene.toneMapper.maxLuminance * scene.toneMapper.maxLuminance;
	double invGamma = 1.0 / scene.toneMapper.gamma;

	#pragma omp parallel for
	for (int64_t i = 0; i < pixelCount; ++i)
	{
		// fix static analysis warnings
		size_t j = size_t(i);

		double originalLuminance = inputPixelData.at(j).toColor().getLuminance();
		double scaledLuminance = scale * originalLuminance;
		double mappedLuminance = (scaledLuminance * (1.0 + (scaledLuminance / maxLuminance2))) / (1.0 + scaledLuminance);
		double colorScale = mappedLuminance / originalLuminance;

		outputPixelData[j] = (inputPixelData.at(j).toColor() * colorScale).toColorf();
		outputPixelData[j].a = 1.0;

		if (scene.toneMapper.applyGamma)
			outputPixelData[j] = Color::pow(outputPixelData[j].toColor(), invGamma).toColorf();

		if (scene.toneMapper.shouldClamp)
			outputPixelData[j].toColor().clamp().toColorf();
	}
}
