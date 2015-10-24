// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Math/Vector3.h"

namespace Raycer
{
	class Ray;
	class EulerAngle;

	class AABB
	{
	public:

		AABB();

		static AABB createFromMinMax(const Vector3& min, const Vector3& max);
		static AABB createFromCenterExtent(const Vector3& center, const Vector3& extent);
		static AABB createFromVertices(const Vector3& v0, const Vector3& v1, const Vector3& v2);

		bool intersects(const Ray& ray) const;
		void expand(const AABB& other);
		uint64_t getLargestAxis() const;
		AABB transformed(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate) const;

		Vector3 getMin() const;
		Vector3 getMax() const;
		Vector3 getCenter() const;
		Vector3 getExtent() const;
		double getSurfaceArea() const;

	private:

		Vector3 min;
		Vector3 max;

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(min),
				CEREAL_NVP(max));
		}
	};
}
