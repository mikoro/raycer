// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>
#include <string>

/*

Read mesh data from PLY formatted files. Both ascii and binary is supported.

Following names are assumed:

Element "vertex":
x
y
z
nx (optional)
ny (optional)
nz (optional)
s (optional)
t (optional)

Element "face":
vertex_indices (list, 3 or more vertex indices, polygons are triangulated)

*/

namespace Raycer
{
	class Triangle;

	class PlyReader
	{
	public:

		static std::vector<Triangle> readFile(const std::string& fileName);
	};
}
