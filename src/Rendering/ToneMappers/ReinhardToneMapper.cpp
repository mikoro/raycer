// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/ToneMappers/ReinhardToneMapper.h"
#include "Raytracing/Scene.h"
#include "Rendering/Image.h"
#include "Math/Color.h"

using namespace Raycer;

ReinhardToneMapper::ReinhardToneMapper()
{
	maxLuminanceAverage.setAverage(1.0);
}

void ReinhardToneMapper::apply(const Scene& scene, const Image& inputImage, Image& outputImage)
{
	const AlignedColorfVector& inputPixelData = inputImage.getPixelDataConst();
	AlignedColorfVector& outputPixelData = outputImage.getPixelData();

	const double epsilon = std::numeric_limits<double>::epsilon();
	const int64_t pixelCount = int64_t(inputPixelData.size());
	double luminanceLogSum = 0.0;
	double maxLuminance = 1.0;
	double maxLuminancePrivate;

	#pragma omp parallel reduction(+ : luminanceLogSum) private(maxLuminancePrivate)
	{
		maxLuminancePrivate = 0.0;

		#pragma omp for
		for (int64_t i = 0; i < pixelCount; ++i)
		{
			double luminance = inputPixelData.at(i).toColor().getLuminance();
			luminanceLogSum += log(epsilon + luminance);

			if (luminance > maxLuminancePrivate)
				maxLuminancePrivate = luminance;
		}

		if (maxLuminancePrivate > maxLuminance)
		{
			#pragma omp critical
			{
				if (maxLuminancePrivate > maxLuminance)
					maxLuminance = maxLuminancePrivate;
			}
		}
	}

	if (scene.toneMapper.enableAveraging)
	{
		maxLuminanceAverage.setAlpha(scene.toneMapper.averageAlpha);
		maxLuminanceAverage.addMeasurement(maxLuminance);
		maxLuminance = maxLuminanceAverage.getAverage();
	}

	const double luminanceLogAvg = exp(luminanceLogSum / double(pixelCount));
	const double scale = scene.toneMapper.key / luminanceLogAvg;
	const double maxLuminance2Inv = 1.0 / (maxLuminance * maxLuminance);
	const double invGamma = 1.0 / scene.toneMapper.gamma;
	
	#pragma omp parallel for
	for (int64_t i = 0; i < pixelCount; ++i)
	{
		double originalLuminance = inputPixelData.at(i).toColor().getLuminance();
		double scaledLuminance = scale * originalLuminance;
		double mappedLuminance = (scaledLuminance * (1.0 + (scaledLuminance * maxLuminance2Inv))) / (1.0 + scaledLuminance);
		double colorScale = mappedLuminance / originalLuminance;

		outputPixelData[i] = (inputPixelData.at(i).toColor() * colorScale).toColorf();
		outputPixelData[i].a = 1.0;

		if (scene.toneMapper.applyGamma)
			outputPixelData[i] = Color::fastPow(outputPixelData[i].toColor(), invGamma).toColorf();

		if (scene.toneMapper.shouldClamp)
			outputPixelData[i].toColor().clamp().toColorf();
	}
}
