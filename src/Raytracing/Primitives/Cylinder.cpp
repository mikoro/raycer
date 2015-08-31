// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>
#include <limits>

#include "Raytracing/Primitives/Cylinder.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Raytracing/ONB.h"
#include "Math/Vector2.h"
#include "Math/Matrix4x4.h"
#include "Math/Solver.h"

using namespace Raycer;

void Cylinder::initialize()
{
}

bool Cylinder::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	double a = ray.direction.x * ray.direction.x + ray.direction.z * ray.direction.z;
	double b = 2.0 * (ray.origin.x * ray.direction.x + ray.origin.z * ray.direction.z);
	double c = ray.origin.x * ray.origin.x + ray.origin.z * ray.origin.z - radius * radius;

	QuadraticResult result = Solver::quadratic(a, b, c);

	if (result.rootCount == 0)
		return false;

	if (result.roots[0] < 0.0 && result.roots[1] < 0.0)
		return false;

	auto calculateIntersection = [&](double t, CSGDirection direction)
	{
		Intersection tempIntersection;

		Vector3 ip = ray.origin + (t * ray.direction);

		if (ip.y < 0.0 || ip.y > height)
			return tempIntersection;

		Vector3 normal = Vector3(ip.x, 0.0, ip.z).normalized();

		tempIntersection.wasFound = true;
		tempIntersection.distance = t;
		tempIntersection.primitive = this;
		tempIntersection.position = ip;
		tempIntersection.normal = normal;
		tempIntersection.onb = ONB::fromNormal(tempIntersection.normal);
		tempIntersection.direction = direction;

		return tempIntersection;
	};

	auto checkIntersection = [&](const Intersection& tempIntersection)
	{
		if (!tempIntersection.wasFound)
			return false;

		if (tempIntersection.distance < 0.0)
			return false;

		if (tempIntersection.distance < ray.minDistance || tempIntersection.distance > ray.maxDistance)
			return false;

		if (tempIntersection.distance > intersection.distance)
			return false;

		return true;
	};

	// CSG intersections
	Intersection tempIntersection1 = calculateIntersection(result.roots[0], CSGDirection::IN);
	Intersection tempIntersection2 = calculateIntersection(result.roots[1], CSGDirection::OUT);

	if (tempIntersection1.wasFound)
		intersections.push_back(tempIntersection1);

	if (tempIntersection2.wasFound)
		intersections.push_back(tempIntersection2);

	// default intersection
	if (checkIntersection(tempIntersection1))
	{
		intersection = tempIntersection1;
		return true;
	}
	else if (checkIntersection(tempIntersection2))
	{
		intersection = tempIntersection2;
		return true;
	}
	else
		return false;
}

AABB Cylinder::getAABB() const
{
	return AABB::createFromMinMax(Vector3(-radius, 0.0, radius), Vector3(radius, height, -radius));
}

void Cylinder::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	(void)scale;
	(void)rotate;
	(void)translate;
}
