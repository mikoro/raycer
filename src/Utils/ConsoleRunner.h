// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

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

	};
}
