// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

namespace Raycer
{
	class Image;
	class Scene;

	struct TracerState
	{
		TracerState() : pixelsProcessed(0) {};

		Scene* scene = nullptr;

		Image* cumulativeImage = nullptr;
		Image* linearImage = nullptr;
		Image* toneMappedImage = nullptr;

		size_t imageWidth = 0;
		size_t imageHeight = 0;
		size_t pixelStartOffset = 0;
		size_t pixelCount = 0;

		size_t cumulativeSampleCount = 0;

		std::atomic<size_t> pixelsProcessed;
	};
}
