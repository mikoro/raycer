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

	auto evaluate = [&](double t)
	{
		Vector3 point = ray.origin + t * ray.direction;

		double sum = 0.0;

		for (const Vector3& blobPosition : blobPositions)
		{
			double distance = (point - blobPosition).length();
			sum += exp((-blobbiness / radius) * distance + blobbiness);
		}

		return sum - threshold;
	};

	std::vector<double> distances;
	distances.reserve(blobPositions.size() + 1);
	distances.push_back(0.0);

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

	for (int i = 1; i < (int)distances.size(); ++i)
	{
		// sign has changed
		if ((evaluate(distances[i - 1]) < 0.0) != (evaluate(distances[i]) < 0.0))
		{
			t = Solver::findRoot(evaluate, distances[i - 1], distances[i], 32);
			wasFound = true;
			break;
		}
	}

	if (!wasFound)
		return false;

	intersection.wasFound = true;
	intersection.distance = t;
	intersection.primitive = this;

	return true;
}

AABB BlinnBlob::getAABB() const
{
	return AABB();
}

void BlinnBlob::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	(void)scale;
	(void)rotate;
	(void)translate;
}

double BlinnBlob::evaluate(const Vector3& point) const
{
	double sum = 0.0;

	for (const Vector3& blobPosition : blobPositions)
	{
		double distance = (point - blobPosition).length();
		sum += exp((blobbiness / radius) * distance - blobbiness);
	}

	return sum - threshold;
}
