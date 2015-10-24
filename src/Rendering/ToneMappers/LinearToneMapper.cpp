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
	const AlignedColorfVector& inputPixelData = inputImage.getPixelDataConst();
	AlignedColorfVector& outputPixelData = outputImage.getPixelData();

	double invGamma = 1.0 / scene.toneMapper.gamma;
	
	#pragma omp parallel for
	for (int64_t i = 0; i < int64_t(inputPixelData.size()); ++i)
	{
		// fix static analysis warnings
		size_t j = size_t(i);

		outputPixelData[j] = (inputPixelData.at(j).toColor() * pow(2.0, scene.toneMapper.exposure)).toColorf();
		outputPixelData[j].a = 1.0;

		if (scene.toneMapper.applyGamma)
			outputPixelData[j] = Color::pow(outputPixelData[j].toColor(), invGamma).toColorf();

		if (scene.toneMapper.shouldClamp)
			outputPixelData[j].toColor().clamp().toColorf();
	}
}
