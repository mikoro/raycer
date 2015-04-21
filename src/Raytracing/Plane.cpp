// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <limits>
#include <cmath>

#include "Raytracing/Plane.h"
#include "Raytracing/Ray.h"

using namespace Raycer;

void Plane::initialize()
{
	uAxis = Vector3(normal.y, normal.z, -normal.x).normalized();
	vAxis = uAxis.cross(normal).normalized();
}

void Plane::intersect(Ray& ray) const
{
	const double epsilon = std::numeric_limits<double>::epsilon();

	double denominator = ray.direction.dot(normal);

	if (fabs(denominator) < epsilon)
		return;

	double t = (position - ray.origin).dot(normal) / denominator;

	if (t < 0.0)
		return;

	if (t > ray.intersection.distance)
		return;

	Vector3 intersectionPosition = ray.origin + (t * ray.direction);

	ray.intersection.wasFound = true;
	ray.intersection.distance = t;
	ray.intersection.position = intersectionPosition;
	ray.intersection.normal = (denominator > 0.0) ? -normal : normal;
	ray.intersection.materialId = materialId;

	double u1 = uAxis.dot(intersectionPosition) / texcoordScale.x;
	double v1 = vAxis.dot(intersectionPosition) / texcoordScale.y;
	double u2 = fabs(u1 - (int)u1);
	double v2 = fabs(v1 - (int)v1);

	if (u1 < 0.0)
		u2 = 1.0 - u2;

	if (v1 < 0.0)
		v2 = 1.0 - v2;

	ray.intersection.texcoord.x = u2;
	ray.intersection.texcoord.y = v2;
}
