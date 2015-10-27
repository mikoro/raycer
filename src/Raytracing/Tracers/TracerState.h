// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

namespace Raycer
{
	class Scene;
	class Film;

	struct TracerState
	{
		TracerState() : pixelsProcessed(0) {};

		Scene* scene = nullptr;
		Film* film = nullptr;
		
		uint64_t filmWidth = 0;
		uint64_t filmHeight = 0;
		uint64_t pixelStartOffset = 0;
		uint64_t pixelCount = 0;

		std::atomic<uint64_t> pixelsProcessed;
	};
}
