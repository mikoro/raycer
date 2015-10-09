// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/ToneMappers/LinearToneMapper.h"
#include "Raytracing/Scene.h"
#include "Rendering/Image.h"
#include "Math/Color.h"

using namespace Raycer;

void LinearToneMapper::apply(const Scene& scene, const Image& inputImage, Image& outputImage)
{
	const std::vector<Color>& inputPixelData = inputImage.getPixelDataConst();
	std::vector<Color>& outputPixelData = outputImage.getPixelData();

	double invGamma = 1.0 / scene.toneMapper.gamma;
	int pixelCount = int(inputPixelData.size());

	#pragma omp parallel for
	for (int i = 0; i < pixelCount; ++i)
	{
		// fix static analysis warnings
		size_t j = size_t(i);

		outputPixelData[j] = inputPixelData.at(j) * pow(2.0, scene.toneMapper.exposure);
		outputPixelData[j].a = 1.0;

		if (scene.toneMapper.applyGamma)
			outputPixelData[j] = Color::pow(outputPixelData[j], invGamma);

		if (scene.toneMapper.shouldClamp)
			outputPixelData[j].clamp();
	}
}
