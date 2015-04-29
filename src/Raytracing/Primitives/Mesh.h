// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>
#include <vector>

#include "Raytracing/Primitives/Primitive.h"
#include "Raytracing/Primitives/Triangle.h"
#include "Math/Vector3.h"
#include "Math/EulerAngle.h"

namespace Raycer
{
	class Mesh : public Primitive
	{
	public:

		void initialize();
		void intersect(Ray& ray) const;

		std::string meshFilePath;
		Vector3 position;
		EulerAngle orientation;

		std::vector<Triangle> triangles;
	};
}
