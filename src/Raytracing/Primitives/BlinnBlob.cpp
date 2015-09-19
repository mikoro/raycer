// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cmath>
#include <limits>

#include "Raytracing/Primitives/BlinnBlob.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Raytracing/ONB.h"
#include "Math/Vector2.h"
#include "Math/Matrix4x4.h"
#include "Math/Solver.h"

using namespace Raycer;

void BlinnBlob::initialize()
{
}

bool BlinnBlob::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
{
	(void)intersections;

	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	// evaluate the effect of all blobs at the given point of the ray
	auto evaluate = [&](double t)
	{
		Vector3 point = ray.origin + t * ray.direction;

		double sum = 0.0;

		for (const Vector3& blobPosition : blobPositions)
		{
			double distance = (point - blobPosition).length();
			sum += exp(-blobbiness / radius * distance + blobbiness);
		}

		return sum - threshold;
	};

	std::vector<double> distances;
	distances.reserve(blobPositions.size() + 1);
	distances.push_back(0.0);

	// TODO: find better distances/positions for the root search
	// project blob positions to the ray
	for (const Vector3& blobPosition : blobPositions)
	{
		Vector3 originToBlob = (blobPosition - ray.origin);
		double distance = originToBlob.dot(ray.direction);

		if (distance > 0.0)
			distances.push_back(originToBlob.dot(ray.direction));
	}

	if (distances.size() < 2)
		return false;

	std::sort(distances.begin(), distances.end());
	double t = 0.0;
	bool wasFound = false;

	// find the interval where the function first changes sign and then solve the exact root/distance
	for (int i = 1; i < (int)distances.size(); ++i)
	{
		if ((evaluate(distances[i - 1]) < 0.0) != (evaluate(distances[i]) < 0.0))
		{
			t = Solver::findRoot(evaluate, distances[i - 1], distances[i], 32);
			wasFound = true;
			break;
		}
	}

	if (!wasFound)
		return false;

	if (!ray.isInstanceRay)
	{
		if (t < ray.minDistance || t > ray.maxDistance)
			return false;

		if (t > intersection.distance)
			return false;
	}

	Vector3 ip = ray.origin + (t * ray.direction);
	Vector3 normal;

	// calculate normal from gradient
	for (const Vector3& blobPosition : blobPositions)
	{
		double distance = (blobPosition - ip).length();
		double temp1 = -blobbiness / (radius * distance);
		double temp2 = exp(-blobbiness / radius * distance + blobbiness);

		normal.x += temp1 * (ip.x - blobPosition.x) * temp2;
		normal.y += temp1 * (ip.y - blobPosition.y) * temp2;
		normal.z += temp1 * (ip.z - blobPosition.z) * temp2;
	}

	normal *= -1.0;
	normal.normalize();
	
	intersection.wasFound = true;
	intersection.distance = t;
	intersection.primitive = this;
	intersection.position = ip;
	intersection.normal = normal;
	intersection.onb = ONB::fromNormal(normal);

	return true;
}

AABB BlinnBlob::getAABB() const
{
	AABB aabb;

	for (const Vector3& blobPosition : blobPositions)
		aabb.expand(AABB::createFromCenterExtent(blobPosition, Vector3(radius * 2.0, radius * 2.0, radius * 2.0)));

	return aabb;
}

void BlinnBlob::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	(void)scale;
	(void)rotate;
	(void)translate;
}
