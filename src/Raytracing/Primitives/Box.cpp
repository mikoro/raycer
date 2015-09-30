// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Primitives/Box.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Raytracing/ONB.h"
#include "Math/EulerAngle.h"

using namespace Raycer;

void Raycer::Box::initialize(const Scene& scene)
{
	(void)scene;

	aabb = AABB::createFromCenterExtent(position, extent);
}

// http://tavianator.com/fast-branchless-raybounding-box-intersections-part-2-nans/
// http://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
bool Box::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
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

	auto calculateIntersection = [&](double t, const Vector3& normal, CSGDirection direction)
	{
		Intersection tempIntersection;

		tempIntersection.wasFound = true;
		tempIntersection.distance = t;
		tempIntersection.primitive = this;
		tempIntersection.position = ray.origin + (t * ray.direction);
		tempIntersection.normal = (direction == CSGDirection::IN) ? normal : -normal;
		tempIntersection.normal = material->invertNormal ? -tempIntersection.normal : tempIntersection.normal;
		tempIntersection.onb = ONB::fromNormal(tempIntersection.normal);
		tempIntersection.direction = direction;

		// TODO: texcoord calculation

		return tempIntersection;
	};

	if (material->enableCSG)
	{
		// CSG intersections
		intersections.push_back(calculateIntersection(tmin, minNormal, CSGDirection::IN));
		intersections.push_back(calculateIntersection(tmax, maxNormal, CSGDirection::OUT));
	}
	
	// default intersection
	bool isInside = (tmin < 0.0);
	double t = isInside ? tmax : tmin;

	if (t < ray.minDistance || t > ray.maxDistance)
		return false;

	if (t > intersection.distance)
		return false;

	intersection = calculateIntersection(t, isInside ? maxNormal : minNormal, CSGDirection::IN);

	return true;
}

AABB Box::getAABB() const
{
	return aabb;
}

void Box::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	(void)rotate;

	position += translate;
	extent *= scale;

	aabb = AABB::createFromCenterExtent(position, extent);
}
