// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

#include <GL/glew.h>

namespace Raycer
{
	class OpenGL
	{
	public:

		static GLuint buildProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		
	};
}
