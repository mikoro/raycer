// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

namespace Raycer
{
	struct ConsoleSettings
	{
		std::string sceneFileName;
		std::string outputFileName;
		int width;
		int height;
		bool viewImage;
	};
}
