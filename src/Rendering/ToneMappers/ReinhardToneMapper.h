// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Rendering/ToneMappers/ToneMapper.h"

// https://www.cs.utah.edu/~reinhard/cdrom/tonemap.pdf

namespace Raycer
{
	class Scene;
	class Image;

	class ReinhardToneMapper : public ToneMapper
	{
	public:

		void apply(const Scene& scene, const Image& inputImage, Image& outputImage) override;
	};
}
