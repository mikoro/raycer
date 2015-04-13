// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <chrono>
#include <atomic>

#include "Rendering/Image.h"

namespace Raycer
{
	struct RaytraceInfo;

	class ConsoleRunner
	{
	public:

		int run();
		void run(RaytraceInfo& info);
		void interrupt();

		Image& getResultImage();

	private:

		void printProgress(const std::chrono::time_point<std::chrono::system_clock>& startTime, size_t totalPixelCount, size_t pixelsProcessed, size_t raysProcessed);

		bool openCLInitialized = false;
		std::atomic<bool> interrupted;
		Image resultImage;
	};
}
