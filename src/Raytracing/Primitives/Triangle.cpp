// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Raytracing/Primitives/Triangle.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"

using namespace Raycer;

// Möller-Trumbore algorithm
// http://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
bool Triangle::intersect(Ray& ray) const
{
	const double epsilon = std::numeric_limits<double>::epsilon();

	Vector3 v0v1 = vertices[1] - vertices[0];
	Vector3 v0v2 = vertices[2] - vertices[0];

	Vector3 pvec = ray.direction.cross(v0v2);
	double determinant = v0v1.dot(pvec);

	// ray and triangle are parallel -> no intersection
	if (fabs(determinant) < epsilon)
		return false;

	double invDeterminant = 1.0 / determinant;

	Vector3 tvec = ray.origin - vertices[0];
	double u = tvec.dot(pvec) * invDeterminant;

	if (u < 0.0 || u > 1.0)
		return false;

	Vector3 qvec = tvec.cross(v0v1);
	double v = ray.direction.dot(qvec) * invDeterminant;

	if (v < 0.0 || (u + v) > 1.0)
		return false;

	double t = v0v2.dot(qvec) * invDeterminant;

	// intersection is behind ray origin
	if (t < 0.0)
		return false;

	// another intersection is closer
	if (t > ray.intersection.distance)
		return false;

	double w = 1.0 - u - v;

	// order has been swapped uvw -> wuv
	Vector3 interpolatedNormal = w * normals[0] + u * normals[1] + v * normals[2];
	Vector2 interpolatedTexcoord = w * texcoords[0] + u * texcoords[1] + v * texcoords[2];

	ray.intersection.wasFound = true;
	ray.intersection.distance = t;
	ray.intersection.position = ray.origin + (t * ray.direction);
	ray.intersection.normal = interpolatedNormal;
	ray.intersection.texcoord = interpolatedTexcoord;

	return true;
}

// http://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
bool Triangle::intersect2(Ray& ray) const
{
	/*
	    (P0-R0) dot N
	t = -------------
	       Rd dot N
	*/

	const double epsilon = std::numeric_limits<double>::epsilon();
	double denominator = ray.direction.dot(normal);

	// ray and triangle are parallel -> no intersection
	if (fabs(denominator) < epsilon)
		return false;

	double t = (vertices[0] - ray.origin).dot(normal) / denominator;

	// intersection is behind ray origin
	if (t < 0.0)
		return false;

	// another intersection is closer
	if (t > ray.intersection.distance)
		return false;

	// intersection position
	Vector3 ip = ray.origin + (t * ray.direction);

	// barycentric coordinates precalc
	Vector3 v0v1 = vertices[1] - vertices[0];
	Vector3 v0v2 = vertices[2] - vertices[0];
	Vector3 normal2 = v0v1.cross(v0v2);

	// edge 0
	Vector3 v0ip = ip - vertices[0];
	Vector3 c0 = v0v1.cross(v0ip);

	if (normal2.dot(c0) < 0.0)
		return false;

	// edge 1
	Vector3 v1v2 = vertices[2] - vertices[1];
	Vector3 v1ip = ip - vertices[1];
	Vector3 c1 = v1v2.cross(v1ip);
	double u = normal2.dot(c1);

	if (u < 0.0)
		return false;

	// edge 2
	Vector3 v2v0 = vertices[0] - vertices[2];
	Vector3 v2ip = ip - vertices[2];
	Vector3 c2 = v2v0.cross(v2ip);
	double v = normal2.dot(c2);

	if (v < 0.0)
		return false;

	double denominator2 = normal2.dot(normal2);
	u /= denominator2;
	v /= denominator2;
	double w = 1.0 - u - v;

	Vector3 interpolatedNormal = u * normals[0] + v * normals[1] + w * normals[2];
	Vector2 interpolatedTexcoord = u * texcoords[0] + v * texcoords[1] + w * texcoords[2];

	ray.intersection.wasFound = true;
	ray.intersection.distance = t;
	ray.intersection.position = ip;
	ray.intersection.normal = interpolatedNormal;
	ray.intersection.texcoord = interpolatedTexcoord;

	return true;
}
