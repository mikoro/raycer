// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Rendering/ToneMappers/ToneMapper.h"

namespace Raycer
{
	class Image;

	class ReinhardToneMapper : public ToneMapper
	{
	public:

		ReinhardToneMapper(bool applyGamma = true, double gamma = 2.2, bool shouldClamp = false, double key = 0.18, double maxLuminance = 1.0);

		void apply(Image& image);

	private:

		double key;
		double maxLuminance;
	};
}
