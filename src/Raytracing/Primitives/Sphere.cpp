// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Raytracing/Primitives/Sphere.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Raytracing/ONB.h"
#include "Math/Vector2.h"
#include "Math/EulerAngle.h"

using namespace Raycer;

void Sphere::initialize()
{
}

// http://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
bool Sphere::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	Vector3 rayOriginToSphere = position - ray.origin;
	double rayOriginToSphereDistance2 = rayOriginToSphere.lengthSquared();

	double t1 = rayOriginToSphere.dot(ray.direction);
	double sphereToRayDistance2 = rayOriginToSphereDistance2 - (t1 * t1);
	double radius2 = radius * radius;

	bool rayOriginIsOutside = (rayOriginToSphereDistance2 >= radius2);

	if (rayOriginIsOutside)
	{
		// whole sphere is behind the ray origin
		if (t1 < 0.0)
			return false;

		// ray misses the sphere completely
		if (sphereToRayDistance2 > radius2)
			return false;
	}

	auto calculateIntersection = [&](double t, CSGDirection direction)
	{
		Intersection tempIntersection;

		Vector3 ip = ray.origin + (t * ray.direction);
		Vector3 normal = (ip - position).normalized();

		if (material->invertNormal)
			normal = -normal;

		tempIntersection.wasFound = true;
		tempIntersection.distance = t;
		tempIntersection.primitive = this;
		tempIntersection.position = ip;
		tempIntersection.normal = normal;
		tempIntersection.onb = ONB::fromNormal(tempIntersection.normal);
		tempIntersection.direction = direction;

		double u = 0.5 + atan2(normal.z, normal.x) / (2.0 * M_PI);
		double v = 0.5 - asin(normal.y) / M_PI;

		u /= material->texcoordScale.x;
		v /= material->texcoordScale.y;

		tempIntersection.texcoord.x = u - floor(u);
		tempIntersection.texcoord.y = v - floor(v);

		return tempIntersection;
	};

	double t2 = sqrt(radius2 - sphereToRayDistance2);

	// CSG intersections
	intersections.push_back(calculateIntersection(t1 - t2, CSGDirection::IN));
	intersections.push_back(calculateIntersection(t1 + t2, CSGDirection::OUT));

	// default intersection
	double t = (rayOriginIsOutside) ? (t1 - t2) : (t1 + t2);

	if (t < ray.minDistance || t > ray.maxDistance)
		return false;

	if (t > intersection.distance)
		return false;

	intersection = calculateIntersection(t, CSGDirection::IN);

	return true;
}

AABB Sphere::getAABB() const
{
	return AABB::createFromCenterExtent(position, Vector3(radius * 2.0, radius * 2.0, radius * 2.0));
}

void Sphere::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	(void)rotate;

	radius *= scale.x;
	position += translate;
}
