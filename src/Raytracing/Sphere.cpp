// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Raytracing/Sphere.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"

using namespace Raycer;

Sphere::Sphere()
{
}

Sphere::Sphere(const Vector3& position_, float radius_, const Material& material_) : position(position_), radius(radius_), material(material_)
{
}

void Sphere::setRadius(float radius_)
{
	radius = radius_;
	radius2 = radius * radius;
}

void Sphere::intersect(Ray& ray) const
{
	Vector3 L = position - ray.origin;
	float ta = L.dot(ray.direction);

	// sphere is behind the ray
	if (ta < 0.0f)
		return;

	float d2 = L.lengthSquared() - ta * ta;

	// ray misses the sphere
	if (d2 > radius2)
		return;

	float tb = sqrt(radius2 - d2);
	float t = ta - tb;

	// there was another intersection closer to camera
	if (t > ray.intersection.distance)
		return;

	ray.intersection.wasFound = true;
	ray.intersection.distance = t;
	ray.intersection.position = ray.origin + (t * ray.direction);
	ray.intersection.normal = (ray.intersection.position - position).normalized();
	ray.intersection.material = &material;
}
