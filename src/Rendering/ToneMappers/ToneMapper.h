// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Scene;
	class Image;

	enum class ToneMapperType { NONE, LINEAR, REINHARD };

	class ToneMapper
	{
	public:

		virtual ~ToneMapper()
		{
		}

		virtual void apply(const Scene& scene, Image& image) = 0;
	};
}
