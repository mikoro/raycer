// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>
#include <chrono>

using namespace std::chrono;

namespace Raycer
{
	struct ConsoleRunnerSettings
	{
		std::string sceneFileName;
		std::string outputFileName;
		int width;
		int height;
		bool viewImage;
	};

	class ConsoleRunner
	{
	public:

		static int run(const ConsoleRunnerSettings& settings);

	private:

		static void printProgress(const time_point<system_clock>& startTime, int totalPixelCount, int pixelCount, int rayCount);
	};
}
