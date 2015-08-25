// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Rendering/ToneMappers/ToneMapper.h"

namespace Raycer
{
	class Image;

	class LinearToneMapper : public ToneMapper
	{
	public:

		LinearToneMapper(bool applyGamma = true, double gamma = 2.2, bool shouldClamp = true, double exposure = 0.0);

		void apply(Image& image);

	private:

		double exposure;
	};
}
