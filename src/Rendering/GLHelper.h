// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

#include <GL/glcorearb.h>

namespace Raycer
{
	class GLHelper
	{
	public:

		static GLuint buildProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		static void checkError(const std::string& message);
		static std::string getErrorMessage(int result);
	};
}
