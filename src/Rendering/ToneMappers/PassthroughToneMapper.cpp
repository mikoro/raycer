// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/ToneMappers/PassthroughToneMapper.h"
#include "Raytracing/Scene.h"
#include "Rendering/Image.h"
#include "Math/Color.h"

using namespace Raycer;

void PassthroughToneMapper::apply(const Scene& scene, const Image& inputImage, Image& outputImage)
{
	(void)scene;

	const AlignedColorfVector& inputPixelData = inputImage.getPixelDataConst();
	AlignedColorfVector& outputPixelData = outputImage.getPixelData();

	#pragma omp parallel for
	for (int64_t i = 0; i < int64_t(inputPixelData.size()); ++i)
		outputPixelData[i] = inputPixelData.at(i);
}
