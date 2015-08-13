// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>
#include <limits>

#include "Raytracing/Primitives/Plane.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Math/Vector2.h"

using namespace Raycer;

void Plane::initialize()
{
	uAxis = Vector3(normal.y, normal.z, -normal.x).normalized();
	vAxis = uAxis.cross(normal).normalized();
}

bool Plane::intersect(const Ray& ray, Intersection& intersection) const
{
	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	double denominator = ray.direction.dot(normal);

	// ray and plane are parallel -> no intersection
	if (fabs(denominator) < std::numeric_limits<double>::epsilon())
		return false;

	double t = (position - ray.origin).dot(normal) / denominator;

	if (t < 0.0 || t < ray.tmin || t > ray.tmax)
		return false;

	// another intersection is closer
	if (t > intersection.distance)
		return false;

	intersection.wasFound = true;
	intersection.distance = t;

	if (ray.fastIntersection)
		return true;

	// intersection position
	Vector3 ip = ray.origin + (t * ray.direction);

	intersection.position = ip;
	intersection.normal = normal;
	intersection.materialId = materialId;

	// texture coordinate calculation
	double u = uAxis.dot(ip) / texcoordScale.x;
	double v = vAxis.dot(ip) / texcoordScale.y;
	intersection.texcoord.x = fabs(u - floor(u));
	intersection.texcoord.y = fabs(v - floor(v));

	return true;
}

AABB Plane::getAABB() const
{
	// TODO: create bounds for the plane aabb
	return AABB(Vector3(-1000.0, -1000.0, -1000.0), Vector3(1000.0, 1000.0, 1000.0));
}
