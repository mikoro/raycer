// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/ToneMappers/SimpleToneMapper.h"
#include "Raytracing/Scene.h"
#include "Rendering/Image.h"
#include "Math/Color.h"
#include "Math/MathUtils.h"

using namespace Raycer;

void SimpleToneMapper::apply(const Scene& scene, const Image& inputImage, Image& outputImage)
{
	const AlignedColorfVector& inputPixelData = inputImage.getPixelDataConst();
	AlignedColorfVector& outputPixelData = outputImage.getPixelData();

	const double invGamma = 1.0 / scene.toneMapper.gamma;

	#pragma omp parallel for
	for (int64_t i = 0; i < int64_t(inputPixelData.size()); ++i)
	{
		Color inputColor = inputPixelData.at(i).toColor();
		Color outputColor = inputColor * MathUtils::fastPow(2.0, scene.toneMapper.exposure);
		outputColor = (outputColor / (Color(1.0, 1.0, 1.0, 1.0) + outputColor));

		if (scene.toneMapper.applyGamma)
			outputColor = Color::fastPow(outputColor, invGamma);

		if (scene.toneMapper.shouldClamp)
			outputColor.clamp();

		outputColor.a = 1.0;
		outputPixelData[i] = outputColor.toColorf();
	}
}
