// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Raytracing/Primitives/Triangle.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"

using namespace Raycer;

void Triangle::initialize()
{
}

void Triangle::intersect(Ray& ray) const
{
	// http://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution

	/*
	    (P0-R0) dot N
	t = -------------
	       Rd dot N
	*/

	const double epsilon = std::numeric_limits<double>::epsilon();

	double denominator = ray.direction.dot(normal);

	// ray and triangle are parallel -> no intersection
	if (fabs(denominator) < epsilon)
		return;

	double t = (vertices[0] - ray.origin).dot(normal) / denominator;

	// intersection is behind ray origin
	if (t < 0.0)
		return;

	// another intersection is closer
	if (t > ray.intersection.distance)
		return;

	// intersection position
	Vector3 ip = ray.origin + (t * ray.direction);

	// edge 0
	Vector3 v0v1 = vertices[1] - vertices[0];
	Vector3 v0ip = ip - vertices[0];
	Vector3 c0 = v0v1.cross(v0ip);

	if (normal.dot(c0) < 0.0)
		return;

	// edge 1
	Vector3 v1v2 = vertices[2] - vertices[1];
	Vector3 v1ip = ip - vertices[1];
	Vector3 c1 = v1v2.cross(v1ip);

	if (normal.dot(c1) < 0.0)
		return;

	// edge 2
	Vector3 v2v0 = vertices[0] - vertices[2];
	Vector3 v2ip = ip - vertices[2];
	Vector3 c2 = v2v0.cross(v2ip);

	if (normal.dot(c2) < 0.0)
		return;

	ray.intersection.wasFound = true;
	ray.intersection.distance = t;
	ray.intersection.position = ip;
	ray.intersection.normal = (denominator < 0.0) ? normal : -normal;
	ray.intersection.materialId = materialId;
}
