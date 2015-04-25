// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Raytracing/Primitive.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Plane : public Primitive
	{
	public:

		void initialize();
		void intersect(Ray& ray) const;

		Vector3 position = Vector3(0.0, 0.0, 0.0);
		Vector3 normal = Vector3(0.0, 1.0, 0.0);

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(position),
				CEREAL_NVP(normal),
				CEREAL_NVP(materialId),
				CEREAL_NVP(texcoordScale));
		}

	private:

		Vector3 uAxis = Vector3(0.0, 0.0, 0.0);
		Vector3 vAxis = Vector3(0.0, 0.0, 0.0);
	};
}
