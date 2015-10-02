// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Primitives/BlinnBlob.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Raytracing/ONB.h"
#include "Math/Vector2.h"
#include "Math/Matrix4x4.h"
#include "Math/Solver.h"

using namespace Raycer;

void Raycer::BlinnBlob::initialize(const Scene& scene)
{
	(void)scene;

	for (const BlinnBlobDescription& blob : blobs)
	{
		if (!blob.isNegative)
			aabb.expand(AABB::createFromCenterExtent(blob.position, Vector3(blob.radius * 2.0, blob.radius * 2.0, blob.radius * 2.0)));
	}
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

		for (const BlinnBlobDescription& blob : blobs)
		{
			double distance = (point - blob.position).length();
			sum += exp(-blob.blobbiness / blob.radius * distance + blob.blobbiness) * (blob.isNegative ? -1.0 : 1.0);
		}

		// 1.0 is the threshold and is defined to be one
		return sum - 1.0;
	};

	std::vector<double> distances;
	distances.reserve(blobs.size() * 3);

	// TODO: doesn't work for negative blobs (the whole premise of finding bracket positions with simple sphere intersection is probably flawed)
	// find bracketing positions for the root search
	for (const BlinnBlobDescription& blob : blobs)
	{
		Vector3 oMinusC = ray.origin - blob.position;
		double a = 1.0;
		double b = 2.0 * ray.direction.dot(oMinusC);
		double c = oMinusC.dot(oMinusC) - blob.radius * blob.radius;
		QuadraticResult result = Solver::findQuadraticRoots(a, b, c);

		// if ray intersects the sphere
		if (result.rootCount == 2 && result.roots[0] > 0.0)
		{
			distances.push_back(result.roots[0]);
			distances.push_back(result.roots[1]);
		}

		// project sphere center to ray
		Vector3 originToBlob = (blob.position - ray.origin);
		double distance = originToBlob.dot(ray.direction);

		if (distance > 0.0)
			distances.push_back(distance);
	}

	if (distances.size() == 0)
		return false;

	std::sort(distances.begin(), distances.end());
	double t = std::numeric_limits<double>::max();
	bool wasFound = false;

	// find the position where the function becomes positive and then solve the exact root/distance
	for (int i = 0; i < (int)distances.size(); ++i)
	{
		if (evaluate(distances[i]) > 0.0)
		{
			t = Solver::findRoot(evaluate, 0.0, distances[i], solverIterations);
			wasFound = true;
			break;
		}
	}

	if (!wasFound)
		return false;

	if (t < ray.minDistance || t > ray.maxDistance)
		return false;

	if (t > intersection.distance)
		return false;

	Vector3 ip = ray.origin + (t * ray.direction);
	Vector3 normal;

	// calculate normal from gradient
	for (const BlinnBlobDescription& blob : blobs)
	{
		double distance = (blob.position - ip).length();
		double temp1 = -blob.blobbiness / (blob.radius * distance) * (blob.isNegative ? -1.0 : 1.0);
		double temp2 = exp(-blob.blobbiness / blob.radius * distance + blob.blobbiness);

		normal.x += temp1 * (ip.x - blob.position.x) * temp2;
		normal.y += temp1 * (ip.y - blob.position.y) * temp2;
		normal.z += temp1 * (ip.z - blob.position.z) * temp2;
	}

	normal *= -1.0;
	normal.normalize();

	intersection.wasFound = true;
	intersection.distance = t;
	intersection.primitive = this;
	intersection.position = ip;
	intersection.normal = material->invertNormal ? -normal : normal;
	intersection.onb = ONB::fromNormal(intersection.normal);

	return true;
}

AABB BlinnBlob::getAABB() const
{
	return aabb;
}

void BlinnBlob::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	(void)scale;
	(void)rotate;
	(void)translate;
}
