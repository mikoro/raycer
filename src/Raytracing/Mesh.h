// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>
#include <vector>

#include "cereal/cereal.hpp"

#include "Raytracing/Primitive.h"
#include "Math/Vector3.h"
#include "Math/EulerAngle.h"
#include "Math/Triangle.h"

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

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(meshFilePath),
				CEREAL_NVP(position),
				CEREAL_NVP(orientation),
				CEREAL_NVP(materialId),
				CEREAL_NVP(texcoordScale));
		}
	};
}
