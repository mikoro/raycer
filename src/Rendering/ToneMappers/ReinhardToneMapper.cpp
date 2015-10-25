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

	const double epsilon = std::numeric_limits<double>::epsilon();
	const int64_t pixelCount = int64_t(inputPixelData.size());
	double luminanceLogSum = 0.0;
	//double maxLuminance = 0.0;

	#pragma omp parallel for reduction(+:luminanceLogSum)
	for (int64_t i = 0; i < pixelCount; ++i)
		luminanceLogSum += log(epsilon + inputPixelData.at(uint64_t(i)).toColor().getLuminance());

	double luminanceLogAvg = exp(luminanceLogSum / double(pixelCount));
	double scale = scene.toneMapper.key / luminanceLogAvg;
	double maxLuminance2 = scene.toneMapper.maxLuminance * scene.toneMapper.maxLuminance;
	double invGamma = 1.0 / scene.toneMapper.gamma;

	#pragma omp parallel for
	for (int64_t i = 0; i < pixelCount; ++i)
	{
		double originalLuminance = inputPixelData.at(i).toColor().getLuminance();
		double scaledLuminance = scale * originalLuminance;
		double mappedLuminance = (scaledLuminance * (1.0 + (scaledLuminance / maxLuminance2))) / (1.0 + scaledLuminance);
		double colorScale = mappedLuminance / originalLuminance;

		outputPixelData[i] = (inputPixelData.at(i).toColor() * colorScale).toColorf();
		outputPixelData[i].a = 1.0;

		if (scene.toneMapper.applyGamma)
			outputPixelData[i] = Color::fastPow(outputPixelData[i].toColor(), invGamma).toColorf();

		if (scene.toneMapper.shouldClamp)
			outputPixelData[i].toColor().clamp().toColorf();
	}
}
