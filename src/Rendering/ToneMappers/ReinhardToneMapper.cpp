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

	const double epsilon = 0.01;
	const int64_t pixelCount = int64_t(inputPixelData.size());
	double luminanceLogSum = 0.0;
	double maxLuminance = 1.0;
	double maxLuminancePrivate;

	#pragma omp parallel reduction(+:luminanceLogSum) private(maxLuminancePrivate)
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
	const double luminanceScale = scene.toneMapper.key / luminanceLogAvg;
	const double maxLuminance2Inv = 1.0 / (maxLuminance * maxLuminance);
	const double invGamma = 1.0 / scene.toneMapper.gamma;
	
	#pragma omp parallel for
	for (int64_t i = 0; i < pixelCount; ++i)
	{
		Color inputColor = inputPixelData.at(i).toColor();

		double originalLuminance = inputColor.getLuminance();
		double scaledLuminance = luminanceScale * originalLuminance;
		double mappedLuminance = (scaledLuminance * (1.0 + (scaledLuminance * maxLuminance2Inv))) / (1.0 + scaledLuminance);
		double colorScale = mappedLuminance / originalLuminance;

		Color outputColor = inputColor * colorScale;

		if (scene.toneMapper.applyGamma)
			outputColor = Color::fastPow(outputColor, invGamma);

		if (scene.toneMapper.shouldClamp)
			outputColor.clamp();

		outputColor.a = 1.0;
		outputPixelData[i] = outputColor.toColorf();
	}
}
