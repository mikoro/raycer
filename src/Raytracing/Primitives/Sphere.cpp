// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Primitives/Sphere.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Math/ONB.h"
#include "Math/Vector2.h"
#include "Math/EulerAngle.h"

using namespace Raycer;

void Raycer::Sphere::initialize(const Scene& scene)
{
	(void)scene;

	aabb = AABB::createFromCenterExtent(position, Vector3(radius * 2.0, radius * 2.0, radius * 2.0));
}

// http://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
bool Sphere::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	Vector3 rayOriginToSphere = position - ray.origin;
	double rayOriginToSphereDistance2 = rayOriginToSphere.lengthSquared();

	double t1 = rayOriginToSphere.dot(ray.direction);
	double sphereToRayDistance2 = rayOriginToSphereDistance2 - (t1 * t1);
	double radius2 = radius * radius;

	bool rayOriginIsOutside = (rayOriginToSphereDistance2 >= radius2);

	if (rayOriginIsOutside)
	{
		// whole sphere is behind the ray origin
		if (t1 < 0.0)
			return false;

		// ray misses the sphere completely
		if (sphereToRayDistance2 > radius2)
			return false;
	}

	auto calculateIntersection = [&](double t, CSGDirection direction)
	{
		Intersection tempIntersection;

		Vector3 ip = ray.origin + (t * ray.direction);
		Vector3 normal = (ip - position).normalized();

		tempIntersection.wasFound = true;
		tempIntersection.distance = t;
		tempIntersection.primitive = this;
		tempIntersection.position = ip;
		tempIntersection.normal = material->invertNormal ? -normal : normal;
		tempIntersection.onb = ONB::fromNormal(tempIntersection.normal);
		tempIntersection.direction = direction;

		double u = 0.0;
		double v = 0.0;

		if (uvMapType == SphereUVMapType::SPHERICAL)
		{
			u = 0.5 - atan2(normal.z, normal.x) / (2.0 * M_PI);
			v = 0.5 + asin(normal.y) / M_PI;
		}
		else if (uvMapType == SphereUVMapType::LIGHT_PROBE)
		{
			double r = (1.0 / M_PI) * acos(normal.z) / sqrt(normal.x * normal.x + normal.y * normal.y);
			u = (r * normal.x + 1.0) / 2.0;
			v = (r * normal.y + 1.0) / 2.0;
		}

		u *= material->texcoordScale.x;
		v *= material->texcoordScale.y;

		tempIntersection.texcoord.x = u - floor(u);
		tempIntersection.texcoord.y = v - floor(v);

		return tempIntersection;
	};

	double t2 = sqrt(radius2 - sphereToRayDistance2);

	if (material->enableCSG)
	{
		// CSG intersections
		intersections.push_back(calculateIntersection(t1 - t2, CSGDirection::IN));
		intersections.push_back(calculateIntersection(t1 + t2, CSGDirection::OUT));
	}

	// default intersection
	double t = (rayOriginIsOutside) ? (t1 - t2) : (t1 + t2);

	if (t < ray.minDistance || t > ray.maxDistance)
		return false;

	if (t > intersection.distance)
		return false;

	intersection = calculateIntersection(t, CSGDirection::IN);

	return true;
}

AABB Sphere::getAABB() const
{
	return aabb;
}

void Sphere::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	(void)rotate;

	radius *= scale.x;
	position += translate;

	aabb = AABB::createFromCenterExtent(position, Vector3(radius * 2.0, radius * 2.0, radius * 2.0));
}
