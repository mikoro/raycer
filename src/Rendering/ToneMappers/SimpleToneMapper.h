// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Rendering/ToneMappers/ToneMapper.h"

namespace Raycer
{
	class Scene;
	class Image;

	class SimpleToneMapper : public ToneMapper
	{
	public:

		void apply(const Scene& scene, const Image& inputImage, Image& outputImage) override;
	};
}
