// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <limits>
#include <cmath>

#include "Raytracing/Primitives/Plane.h"
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

	double u = uAxis.dot(intersectionPosition) / texcoordScale.x;
	double v = vAxis.dot(intersectionPosition) / texcoordScale.y;
	ray.intersection.texcoord.x = fabs(u - floor(u));
	ray.intersection.texcoord.y = fabs(v - floor(v));
}
