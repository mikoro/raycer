// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Primitive.h"
#include "Raytracing/Material.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Plane : public Primitive
	{
	public:

		Plane();
		Plane(const Vector3& position, const Vector3& normal, const Material& material);
		
		void intersect(Ray& ray) const;

		Vector3 position = Vector3(0.0, 0.0, 0.0);
		Vector3 normal = Vector3(0.0, 0.0, 0.0);
		Material material;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(position),
				CEREAL_NVP(normal),
				CEREAL_NVP(material));
		}
	};
}
