// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

namespace Raycer
{
	void checkCLError(int result, const std::string& message);
	void checkGLError(const std::string& message);
	std::string getCLErrorMessage(int result);
	std::string getGLErrorMessage(int result);
}
