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

		uint64_t imageWidth = 0;
		uint64_t imageHeight = 0;
		uint64_t pixelStartOffset = 0;
		uint64_t pixelCount = 0;

		uint64_t cumulativeSampleCount = 0;

		std::atomic<uint64_t> pixelsProcessed;
	};
}
