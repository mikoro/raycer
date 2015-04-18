// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <limits>
#include <cmath>

#include "Raytracing/Plane.h"
#include "Raytracing/Ray.h"

using namespace Raycer;

Plane::Plane()
{
}

Plane::Plane(const Vector3& position_, const Vector3& normal_, const Material& material_) : position(position_), normal(normal_), material(material_)
{
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

	ray.intersection.wasFound = true;
	ray.intersection.distance = t;
	ray.intersection.position = ray.origin + (t * ray.direction);
	ray.intersection.normal = (denominator > 0.0) ? -normal : normal;
	ray.intersection.material = &material;
}
