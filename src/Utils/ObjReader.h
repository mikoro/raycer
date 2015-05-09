// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>
#include <string>

namespace Raycer
{
	class Vector3;
	class Vector2;
	class Triangle;

	class ObjReader
	{
	public:

		static std::vector<Triangle> readFile(const std::string& fileName);

	private:

		static void processFace(std::istringstream& ss, std::vector<Vector3>& vertices, std::vector<Vector2>& texcoords, std::vector<Vector3>& normals, std::vector<Triangle>& triangles);
	};
}
