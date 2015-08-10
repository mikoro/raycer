// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"

namespace Raycer
{
	class Ray;

	class AABB
	{
	public:

		AABB();
		AABB(const Vector3& min, const Vector3& max);

		bool intersects(const Ray& ray) const;
		void expand(const AABB& other);
		void update();
		int getLargestAxis() const;

		Vector3 min;
		Vector3 max;
		Vector3 center;
		Vector3 extent;

		double surfaceArea = 0.0;
	};
}
