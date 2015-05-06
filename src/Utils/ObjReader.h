// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>
#include <string>

namespace Raycer
{
	class Triangle;

	class ObjReader
	{
	public:

		static std::vector<Triangle> readFile(const std::string& fileName);
	};
}
