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
	/*
	    (P0-R0) dot N
	t = -------------
		   Rd dot N
	*/

	const double epsilon = std::numeric_limits<double>::epsilon();

	double denominator = ray.direction.dot(normal);

	// ray and plane are parallel -> no intersection
	if (fabs(denominator) < epsilon)
		return;

	double t = (position - ray.origin).dot(normal) / denominator;

	// intersection is behind ray origin
	if (t < 0.0)
		return;

	// another intersection is closer
	if (t > ray.intersection.distance)
		return;

	// intersection position
	Vector3 ip = ray.origin + (t * ray.direction);

	ray.intersection.wasFound = true;
	ray.intersection.distance = t;
	ray.intersection.position = ip;
	ray.intersection.normal = (denominator < 0.0) ? normal : -normal;
	ray.intersection.materialId = materialId;

	// texture coordinate calculation
	double u = uAxis.dot(ip) / texcoordScale.x;
	double v = vAxis.dot(ip) / texcoordScale.y;
	ray.intersection.texcoord.x = fabs(u - floor(u));
	ray.intersection.texcoord.y = fabs(v - floor(v));
}
