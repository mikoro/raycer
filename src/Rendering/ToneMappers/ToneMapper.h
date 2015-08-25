// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Image;

	enum class ToneMapperType { LINEAR, REINHARD };

	class ToneMapper
	{
	public:

		virtual ~ToneMapper() {}

		virtual void apply(Image& image) = 0;

	protected:

		bool applyGamma;
		double gamma;
		bool shouldClamp;
	};
}
