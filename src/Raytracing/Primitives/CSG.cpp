// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
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

namespace
{
	struct CSGPoint
	{
		Intersection intersection;
		CSGSide side;
		CSGDirection direction;
	};
}

void Raycer::CSG::initialize(const Scene& scene)
{
	(void)scene;
}

bool CSG::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	Intersection leftIntersection;
	Intersection rightIntersection;

	std::vector<Intersection> leftIntersections;
	std::vector<Intersection> rightIntersections;
	std::vector<CSGPoint> csgPoints;

	leftPrimitive->intersect(ray, leftIntersection, leftIntersections);
	rightPrimitive->intersect(ray, rightIntersection, rightIntersections);

	for (const Intersection& tempIntersection : leftIntersections)
	{
		CSGPoint csgPoint;

		csgPoint.intersection = tempIntersection;
		csgPoint.side = CSGSide::LEFT;
		csgPoint.direction = tempIntersection.direction;

		csgPoints.push_back(csgPoint);
	}

	for (const Intersection& tempIntersection : rightIntersections)
	{
		CSGPoint csgPoint;

		csgPoint.intersection = tempIntersection;
		csgPoint.side = CSGSide::RIGHT;
		csgPoint.direction = tempIntersection.direction;

		csgPoints.push_back(csgPoint);
	}

	std::sort(csgPoints.begin(), csgPoints.end(), [](const CSGPoint& p1, const CSGPoint& p2) { return p1.intersection.distance < p2.intersection.distance; });

	CSGDirection leftDirection = CSGDirection::OUT;
	CSGDirection rightDirection = CSGDirection::OUT;

	for (const CSGPoint& csgPoint : csgPoints)
	{
		if (csgPoint.side == CSGSide::LEFT)
			leftDirection = csgPoint.direction;
		else if (csgPoint.side == CSGSide::RIGHT)
			rightDirection = csgPoint.direction;

		CSGDirection newDirection = CSGDirection::OUT;

		if (operation == CSGOperation::UNION)
		{
			if (leftDirection == CSGDirection::IN && rightDirection == CSGDirection::IN)
				newDirection = CSGDirection::IN;
			else if (leftDirection == CSGDirection::IN && rightDirection == CSGDirection::OUT)
				newDirection = CSGDirection::IN;
			else if (leftDirection == CSGDirection::OUT && rightDirection == CSGDirection::IN)
				newDirection = CSGDirection::IN;
			else if (leftDirection == CSGDirection::OUT && rightDirection == CSGDirection::OUT)
				newDirection = CSGDirection::OUT;
		}
		else if (operation == CSGOperation::DIFFERENCE)
		{
			if (leftDirection == CSGDirection::IN && rightDirection == CSGDirection::IN)
				newDirection = CSGDirection::OUT;
			else if (leftDirection == CSGDirection::IN && rightDirection == CSGDirection::OUT)
				newDirection = CSGDirection::IN;
			else if (leftDirection == CSGDirection::OUT && rightDirection == CSGDirection::IN)
				newDirection = CSGDirection::OUT;
			else if (leftDirection == CSGDirection::OUT && rightDirection == CSGDirection::OUT)
				newDirection = CSGDirection::OUT;
		}
		else if (operation == CSGOperation::INTERSECTION)
		{
			if (leftDirection == CSGDirection::IN && rightDirection == CSGDirection::IN)
				newDirection = CSGDirection::IN;
			else if (leftDirection == CSGDirection::IN && rightDirection == CSGDirection::OUT)
				newDirection = CSGDirection::OUT;
			else if (leftDirection == CSGDirection::OUT && rightDirection == CSGDirection::IN)
				newDirection = CSGDirection::OUT;
			else if (leftDirection == CSGDirection::OUT && rightDirection == CSGDirection::OUT)
				newDirection = CSGDirection::OUT;
		}

		Intersection tempIntersection = csgPoint.intersection;
		tempIntersection.direction = newDirection;
		intersections.push_back(tempIntersection);
	}

	for (const Intersection& tempIntersection : intersections)
	{
		if (tempIntersection.direction == CSGDirection::IN)
		{
			double t = tempIntersection.distance;

			if (t < ray.minDistance || t > ray.maxDistance)
				return false;

			if (t > intersection.distance)
				return false;

			if (t < 0.0)
				return false;

			intersection = tempIntersection;
			return true;
		}
	}

	return false;
}

AABB CSG::getAABB() const
{
	AABB aabb;

	aabb.expand(leftPrimitive->getAABB());
	aabb.expand(rightPrimitive->getAABB());

	return aabb;
}

void CSG::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	leftPrimitive->transform(scale, rotate, translate);
	rightPrimitive->transform(scale, rotate, translate);
}
