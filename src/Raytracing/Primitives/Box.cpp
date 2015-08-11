// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cmath>
#include <limits>

#include "Raytracing/Primitives/Box.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"

using namespace Raycer;

Box::Box()
{
}

Box::Box(const AABB& aabb)
{
	min = aabb.min;
	max = aabb.max;
}

void Box::initialize()
{
}

// http://tavianator.com/fast-branchless-raybounding-box-intersections-part-2-nans/
// http://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
void Box::intersect(const Ray& ray, Intersection& intersection) const
{
	double tx0 = (min.x - ray.origin.x) * ray.inverseDirection.x;
	double tx1 = (max.x - ray.origin.x) * ray.inverseDirection.x;

	double tmin = std::min(tx0, tx1);
	double tmax = std::max(tx0, tx1);

	Vector3 minNormal = Vector3(1.0, 0.0, 0.0) * copysign(1.0, ray.direction.x);
	Vector3 maxNormal = Vector3(1.0, 0.0, 0.0) * copysign(1.0, ray.direction.x);

	double ty0 = (min.y - ray.origin.y) * ray.inverseDirection.y;
	double ty1 = (max.y - ray.origin.y) * ray.inverseDirection.y;
	double tymin = std::min(ty0, ty1);
	double tymax = std::max(ty0, ty1);

	if (tymin > tmin)
	{
		minNormal = Vector3(0.0, 1.0, 0.0) * copysign(1.0, ray.direction.y);
		tmin = tymin;
	}

	if (tymax < tmax)
	{
		maxNormal = Vector3(0.0, 1.0, 0.0) * copysign(1.0, ray.direction.y);
		tmax = tymax;
	}

	double tz0 = (min.z - ray.origin.z) * ray.inverseDirection.z;
	double tz1 = (max.z - ray.origin.z) * ray.inverseDirection.z;
	double tzmin = std::min(tz0, tz1);
	double tzmax = std::max(tz0, tz1);

	if (tzmin > tmin)
	{
		minNormal = Vector3(0.0, 0.0, 1.0) * copysign(1.0, ray.direction.z);
		tmin = tzmin;
	}

	if (tzmax < tmax)
	{
		maxNormal = Vector3(0.0, 0.0, 1.0) * copysign(1.0, ray.direction.z);
		tmax = tzmax;
	}

	if (tmax < std::max(tmin, 0.0))
		return;

	bool isInside = (tmin < 0.0);
	double t = isInside ? tmax : tmin;

	if (t > intersection.distance)
		return;

	intersection.wasFound = true;
	intersection.distance = t;
	intersection.position = ray.origin + (t * ray.direction);
	intersection.normal = isInside ? maxNormal : -minNormal;
	intersection.materialId = materialId;

	// TODO: add texcoord mapping
}

AABB Box::getAABB() const
{
	return AABB(min, max);
}

Box Box::create(const Vector3& center, const Vector3& extent)
{
	Box box;

	box.min = center - extent;
	box.max = center + extent;

	return box;
}
