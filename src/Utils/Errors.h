// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

namespace Raycer
{
	void checkClError(int result, const std::string& message);
	void checkGlError(const std::string& message);
	std::string getClErrorMessage(int result);
	std::string getGlErrorMessage(int result);
}
