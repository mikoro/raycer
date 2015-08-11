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

void Plane::intersect(const Ray& ray, Intersection& intersection) const
{
	double denominator = ray.direction.dot(normal);

	// ray and plane are parallel -> no intersection
	if (fabs(denominator) < std::numeric_limits<double>::epsilon())
		return;

	double t = (position - ray.origin).dot(normal) / denominator;

	// intersection is behind ray origin
	if (t < 0.0)
		return;

	// another intersection is closer
	if (t > intersection.distance)
		return;

	// intersection position
	Vector3 ip = ray.origin + (t * ray.direction);

	intersection.wasFound = true;
	intersection.distance = t;
	intersection.position = ip;
	intersection.normal = normal;
	intersection.materialId = materialId;

	// texture coordinate calculation
	double u = uAxis.dot(ip) / texcoordScale.x;
	double v = vAxis.dot(ip) / texcoordScale.y;
	intersection.texcoord.x = fabs(u - floor(u));
	intersection.texcoord.y = fabs(v - floor(v));
}

AABB Plane::getAABB() const
{
	// TODO: create bounds for the plane aabb
	return AABB(Vector3(-100.0, -100.0, -100.0), Vector3(100.0, 100.0, 100.0));
}
