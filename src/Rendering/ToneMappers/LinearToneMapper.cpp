// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/ToneMappers/LinearToneMapper.h"
#include "Raytracing/Scene.h"
#include "Rendering/Image.h"
#include "Math/Color.h"

using namespace Raycer;

void LinearToneMapper::apply(const Scene& scene, Image& image)
{
	std::vector<Color>& pixelData = image.getPixelData();
	double invGamma = 1.0 / scene.toneMapper.gamma;
	int pixelCount = int(pixelData.size());

	#pragma omp parallel for
	for (int i = 0; i < pixelCount; ++i)
	{
		// fix static analysis warnings
		size_t j = size_t(i);

		pixelData[j] *= pow(2.0, scene.toneMapper.exposure);
		pixelData[j].a = 1.0;

		if (scene.toneMapper.applyGamma)
			pixelData[j] = Color::pow(pixelData[j], invGamma);

		if (scene.toneMapper.shouldClamp)
			pixelData[j].clamp();
	}
}
