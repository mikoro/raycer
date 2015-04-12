// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

namespace Raycer
{
	class RenderTarget;
	class Scene;

	struct RaytraceInfo
	{
		RenderTarget* renderTarget = nullptr;
		Scene* scene = nullptr;

		size_t sceneWidth = 0;
		size_t sceneHeight = 0;
		size_t pixelStartOffset = 0;
		size_t pixelTotalCount = 0;

		std::atomic<size_t> pixelsProcessed = 0;
		std::atomic<size_t> raysProcessed = 0;
	};
}
