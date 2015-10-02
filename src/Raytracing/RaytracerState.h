// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

namespace Raycer
{
	class Image;
	class Scene;

	struct RaytracerState
	{
		RaytracerState() : pixelsProcessed(0)
		{
		};

		Image* image = nullptr;
		Scene* scene = nullptr;

		size_t sceneWidth = 0;
		size_t sceneHeight = 0;
		size_t pixelOffset = 0;
		size_t pixelCount = 0;

		std::atomic<size_t> pixelsProcessed;
	};
}
