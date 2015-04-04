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

bool Sphere::intersects(const Ray& ray, Intersection& intersection) const
{
	Vector3 L = position - ray.origin;
	float ta = L.dot(ray.direction);

	// sphere is behind
	if (ta < 0.0f)
		return false;

	float d2 = L.lengthSquared() - ta * ta;

	// ray misses the sphere
	if (d2 > radius2)
		return false;

	float tb = sqrt(radius2 - d2);
	float t = ta - tb;

	//if (t > ray.intersectionDistance)
		//return false;

	intersection.position = ray.origin + (t * ray.direction);
	intersection.normal = (intersection.position - position).normalized();

	return true;
}

const Material& Sphere::getMaterial() const
{
	return material;
}
