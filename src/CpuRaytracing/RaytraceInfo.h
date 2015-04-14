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

		int sceneWidth = 0;
		int sceneHeight = 0;
		int pixelStartOffset = 0;
		int pixelTotalCount = 0;

		std::atomic<int> pixelsProcessed;
		std::atomic<int> raysProcessed;
	};
}
