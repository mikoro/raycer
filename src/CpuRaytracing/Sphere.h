// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "CpuRaytracing/Primitive.h"
#include "CpuRaytracing/Material.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Sphere : public Primitive
	{
	public:

		Sphere();
		Sphere(const Vector3& position, double radius, const Material& material);

		void setRadius(double radius);
		void intersect(Ray& ray) const;

		Vector3 position = Vector3(0.0, 0.0, 0.0);
		Material material;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(position),
				CEREAL_NVP(material),
				CEREAL_NVP(radius));
		}

	private:

		double radius = 1.0;
		double radius2 = 1.0;
	};
}
