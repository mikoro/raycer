// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Raytracing/Primitives/Sphere.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Math/Vector2.h"

using namespace Raycer;

void Sphere::initialize()
{
	radius2 = radius * radius;
}

// http://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
bool Sphere::intersect(const Ray& ray, Intersection& intersection)
{
	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	Vector3 rayOriginToSphere = position - ray.origin;
	double rayOriginToSphereDistance2 = rayOriginToSphere.lengthSquared();

	double t1 = rayOriginToSphere.dot(ray.direction);
	double sphereToRayDistance2 = rayOriginToSphereDistance2 - (t1 * t1);

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

	double t2 = sqrt(radius2 - sphereToRayDistance2);
	double t = (rayOriginIsOutside) ? (t1 - t2) : (t1 + t2);

	if (t < ray.tmin || t > ray.tmax)
		return false;

	// there was another intersection closer to camera
	if (t > intersection.distance)
		return false;

	intersection.wasFound = true;
	intersection.distance = t;
	intersection.primitive = this;

	if (ray.fastIntersection)
		return true;

	// intersection position and normal
	Vector3 ip = ray.origin + (t * ray.direction);
	Vector3 normal = (ip - position).normalized();

	intersection.position = ip;
	intersection.normal = normal;

	// spherical texture coordinate calculation
	double u = 0.5 + atan2(normal.z, normal.x) / (2.0 * M_PI);
	double v = 0.5 - asin(normal.y) / M_PI;
	u /= material->texcoordScale.x;
	v /= material->texcoordScale.y;
	intersection.texcoord.x = u - floor(u);
	intersection.texcoord.y = v - floor(v);

	return true;
}

AABB Sphere::getAABB() const
{
	Vector3 extend = Vector3(radius, radius, radius);
	return AABB(position - extend, position + extend);
}

Vector3* Sphere::getPosition()
{
	return &position;
}
