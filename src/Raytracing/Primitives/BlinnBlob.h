// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Raytracing/Primitives/Primitive.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Ray;
	struct Intersection;
	class AABB;
	class EulerAngle;

	struct BlinnBlobDescription
	{
		Vector3 position;
		double radius = 1.0;
		double blobbiness = 4.0;
		bool isNegative = false;
	};

	class BlinnBlob : public Primitive
	{
	public:

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections);
		AABB getAABB() const;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate);

		std::vector<BlinnBlobDescription> blobs;
		int solverIterations = 16;
	};
}
