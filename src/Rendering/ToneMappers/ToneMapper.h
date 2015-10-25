// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#ifdef PASSTHROUGH
#undef PASSTHROUGH
#endif

namespace Raycer
{
	class Scene;
	class Image;

	enum class ToneMapperType { PASSTHROUGH, LINEAR, SIMPLE, REINHARD };

	class ToneMapper
	{
	public:

		virtual ~ToneMapper() {}

		virtual void apply(const Scene& scene, const Image& inputImage, Image& outputImage) = 0;

		static std::unique_ptr<ToneMapper> getSampler(ToneMapperType type);
	};
}
