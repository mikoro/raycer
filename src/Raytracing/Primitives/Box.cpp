// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cmath>

#include "Raytracing/Primitives/Box.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Raytracing/ONB.h"

using namespace Raycer;

void Box::initialize()
{
}

// http://tavianator.com/fast-branchless-raybounding-box-intersections-part-2-nans/
// http://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
bool Box::intersect(const Ray& ray, std::array<Intersection, 2>& intersections)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersections[0].wasFound)
		return true;

	Vector3 min = position - extent / 2.0;
	Vector3 max = position + extent / 2.0;

	double tx0 = (min.x - ray.origin.x) * ray.inverseDirection.x;
	double tx1 = (max.x - ray.origin.x) * ray.inverseDirection.x;

	double tmin = std::min(tx0, tx1);
	double tmax = std::max(tx0, tx1);

	// normals will point out of the box
	Vector3 minNormal = Vector3(1.0, 0.0, 0.0) * copysign(1.0, -ray.direction.x);
	Vector3 maxNormal = Vector3(1.0, 0.0, 0.0) * copysign(1.0, ray.direction.x);

	double ty0 = (min.y - ray.origin.y) * ray.inverseDirection.y;
	double ty1 = (max.y - ray.origin.y) * ray.inverseDirection.y;
	double tymin = std::min(ty0, ty1);
	double tymax = std::max(ty0, ty1);

	if (tymin > tmin)
	{
		minNormal = Vector3(0.0, 1.0, 0.0) * copysign(1.0, -ray.direction.y);
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
		minNormal = Vector3(0.0, 0.0, 1.0) * copysign(1.0, -ray.direction.z);
		tmin = tzmin;
	}

	if (tzmax < tmax)
	{
		maxNormal = Vector3(0.0, 0.0, 1.0) * copysign(1.0, ray.direction.z);
		tmax = tzmax;
	}

	if (tmax < std::max(tmin, 0.0))
		return false;

	// closer intersection (can be negative)
	double t = tmin;

	if (t < ray.minDistance || t > ray.maxDistance)
		return false;

	if (t > intersections[0].distance)
		return false;

	intersections[0].wasFound = true;
	intersections[0].distance = t;
	intersections[0].primitive = this;
	intersections[0].position = ray.origin + (t * ray.direction);
	intersections[0].normal = minNormal;
	intersections[0].onb = ONB::fromNormal(intersections[0].normal);

	// further intersection (for CSG)
	t = tmax;

	intersections[1].wasFound = true;
	intersections[1].distance = t;
	intersections[1].primitive = this;
	intersections[1].position = ray.origin + (t * ray.direction);
	intersections[1].normal = -maxNormal;
	intersections[1].onb = ONB::fromNormal(intersections[1].normal);

	return true;
}

AABB Box::getAABB() const
{
	return AABB::createFromCenterExtent(position, extent);
}

void Box::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	(void)rotate;

	position += translate;
	extent *= scale;
}
