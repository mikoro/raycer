// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cmath>
#include <limits>

#include "Raytracing/Primitives/Box.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"

using namespace Raycer;

void Box::initialize()
{
}

void Box::intersect(const Ray& ray, Intersection& intersection) const
{
	double t1 = (min.x - ray.origin.x) * ray.inverseDirection.x;
	double t2 = (max.x - ray.origin.x) * ray.inverseDirection.x;

	double tmin = std::min(t1, t2);
	double tmax = std::max(t1, t2);

	t1 = (min.y - ray.origin.y) * ray.inverseDirection.y;
	t2 = (max.y - ray.origin.y) * ray.inverseDirection.y;

	tmin = std::max(tmin, std::min(t1, t2));
	tmax = std::min(tmax, std::max(t1, t2));

	t1 = (min.z - ray.origin.z) * ray.inverseDirection.z;
	t2 = (max.z - ray.origin.z) * ray.inverseDirection.z;

	tmin = std::max(tmin, std::min(t1, t2));
	tmax = std::min(tmax, std::max(t1, t2));

	if (tmax < std::max(tmin, 0.0))
		return;

	if (tmin > intersection.distance)
		return;

	intersection.wasFound = true;
	intersection.distance = tmin;
	intersection.position = ray.origin + (tmin * ray.direction);
	intersection.normal = Vector3();
	intersection.materialId = materialId;
}
