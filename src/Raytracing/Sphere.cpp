// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Raytracing/Sphere.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"

using namespace Raycer;

void Sphere::initialize()
{
}

void Sphere::intersect(Ray& ray) const
{
	Vector3 originToCenter = position - ray.origin;
	double originToSphereDistance2 = originToCenter.lengthSquared();
	double radius2 = radius * radius;

	// ray origin inside the sphere
	if (originToSphereDistance2 < radius2)
		return;

	double ta = originToCenter.dot(ray.direction);

	// sphere is behind the ray
	if (ta < 0.0)
		return;

	double sphereToRayDistance2 = originToSphereDistance2 - (ta * ta);

	// ray misses the sphere
	if (sphereToRayDistance2 > radius2)
		return;

	double tb = sqrt(radius2 - sphereToRayDistance2);
	double t = ta - tb;

	// there was another intersection closer to camera
	if (t > ray.intersection.distance)
		return;

	Vector3 intersectionPosition = ray.origin + (t * ray.direction);
	Vector3 intersectionNormal = (intersectionPosition - position).normalized();

	ray.intersection.wasFound = true;
	ray.intersection.distance = t;
	ray.intersection.position = intersectionPosition;
	ray.intersection.normal = intersectionNormal;
	ray.intersection.materialId = materialId;

	double u = 0.5 + atan2(intersectionNormal.z, intersectionNormal.x) / (2.0 * M_PI);
	double v = 0.5 - asin(intersectionNormal.y) / M_PI;
	u /= texcoordScale.x;
	v /= texcoordScale.y;
	ray.intersection.texcoord.x = u - floor(u);
	ray.intersection.texcoord.y = v - floor(v);
}
