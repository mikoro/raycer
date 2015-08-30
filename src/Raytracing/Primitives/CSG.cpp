// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <array>
#include <cmath>
#include <limits>

#include "Raytracing/Primitives/CSG.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Raytracing/ONB.h"
#include "Math/Vector2.h"
#include "Math/Matrix4x4.h"

using namespace Raycer;

void CSG::initialize()
{
}

bool CSG::intersect(const Ray& ray, std::array<Intersection, 2>& intersections)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersections[0].wasFound)
		return true;

	std::array<Intersection, 2> leftIntersections;
	std::array<Intersection, 2> rightIntersections;
	std::array<Intersection, 2> tempIntersections;

	leftPrimitive->intersect(ray, leftIntersections);
	rightPrimitive->intersect(ray, rightIntersections);

	bool leftWasFound = leftIntersections[0].wasFound && leftIntersections[1].wasFound;
	bool rightWasFound = rightIntersections[0].wasFound && rightIntersections[1].wasFound;

	if (!leftWasFound && !rightWasFound)
		return false;

	auto checkAndAssing = [&](std::array<Intersection, 2>& destination, const std::array<Intersection, 2>& result)
	{
		if (result[0].distance < ray.minDistance || result[0].distance > ray.maxDistance)
			return false;

		if (result[0].distance > intersections[0].distance)
			return false;

		if (result[0].distance < 0.0)
			return false;

		destination[0] = result[0];
		destination[1] = result[1];

		return true;
	};

	// only right was found
	if (!leftWasFound)
	{
		if (operation == CSGOperation::UNION)
			return checkAndAssing(intersections, rightIntersections);
		else
			return false;
	}

	// only left was found
	if (!rightWasFound)
	{
		if (operation == CSGOperation::UNION || operation == CSGOperation::SUBSTRACTION)
			return checkAndAssing(intersections, leftIntersections);
		else
			return false;
	}

	if (operation == CSGOperation::UNION)
	{
		// select intersections furthest apart
		tempIntersections[0] = (leftIntersections[0].distance < rightIntersections[0].distance) ? leftIntersections[0] : rightIntersections[0];
		tempIntersections[1] = (leftIntersections[1].distance > rightIntersections[1].distance) ? leftIntersections[1] : rightIntersections[1];

		return checkAndAssing(intersections, tempIntersections);
	}
	else if (operation == CSGOperation::SUBSTRACTION)
	{
		// right completely substracts left
		if (rightIntersections[0].distance < leftIntersections[0].distance && rightIntersections[1].distance > leftIntersections[1].distance)
			return false;

		// right misses left
		if (rightIntersections[1].distance < leftIntersections[0].distance || rightIntersections[0].distance > leftIntersections[1].distance)
			return checkAndAssing(intersections, leftIntersections);

		// left side
		tempIntersections[0] = (leftIntersections[0].distance > rightIntersections[0].distance) ? rightIntersections[1] : leftIntersections[0];

		// right side
		tempIntersections[1] = (rightIntersections[1].distance > leftIntersections[1].distance) ? rightIntersections[0] : leftIntersections[1];

		return checkAndAssing(intersections, tempIntersections);
	}
	else if (operation == CSGOperation::INTERSECTION)
	{
		// right completely intersects left
		if (rightIntersections[0].distance < leftIntersections[0].distance && rightIntersections[1].distance > leftIntersections[1].distance)
			return checkAndAssing(intersections, leftIntersections);

		// right misses left
		if (rightIntersections[1].distance < leftIntersections[0].distance || rightIntersections[0].distance > leftIntersections[1].distance)
			return false;

		// left side
		tempIntersections[0] = (leftIntersections[0].distance > rightIntersections[0].distance) ? leftIntersections[0] : rightIntersections[0];

		// right side
		tempIntersections[1] = (rightIntersections[1].distance > leftIntersections[1].distance) ? leftIntersections[1] : rightIntersections[1];

		return checkAndAssing(intersections, tempIntersections);
	}

	return false;
}

AABB CSG::getAABB() const
{
	AABB aabb;

	if (leftPrimitive != nullptr)
		aabb.expand(leftPrimitive->getAABB());

	if (rightPrimitive != nullptr)
		aabb.expand(rightPrimitive->getAABB());

	return aabb;
}

void CSG::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	if (leftPrimitive != nullptr)
		leftPrimitive->transform(scale, rotate, translate);

	if (rightPrimitive != nullptr)
		rightPrimitive->transform(scale, rotate, translate);
}
