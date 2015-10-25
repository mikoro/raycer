// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Primitives/Plane.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Math/ONB.h"
#include "Math/Vector2.h"
#include "Math/Matrix4x4.h"

using namespace Raycer;

void Raycer::Plane::initialize(const Scene& scene)
{
	(void)scene;

	uAxis = Vector3(normal.y, normal.z, -normal.x).normalized();
	vAxis = uAxis.cross(normal).normalized();

	double min = -1000000.0;
	double max = 1000000.0;

	aabb = AABB::createFromMinMax(Vector3(min, min, min), Vector3(max, max, max));
}

bool Plane::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
{
	(void)intersections;

	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	double denominator = ray.direction.dot(normal);

	// ray and plane are parallel -> no intersection
	if (std::abs(denominator) < std::numeric_limits<double>::epsilon())
		return false;

	double t = (position - ray.origin).dot(normal) / denominator;

	if (t < 0.0)
		return false;

	if (t < ray.minDistance || t > ray.maxDistance)
		return false;

	if (t > intersection.distance)
		return false;

	// intersection position
	Vector3 ip = ray.origin + (t * ray.direction);

	intersection.wasFound = true;
	intersection.distance = t;
	intersection.primitive = this;
	intersection.position = ip;
	intersection.normal = material->invertNormal ? -normal : normal;
	intersection.onb = ONB::fromNormal(intersection.normal);

	// texture coordinate calculation
	double u = uAxis.dot(ip) * material->texcoordScale.x;
	double v = vAxis.dot(ip) * material->texcoordScale.y;

	intersection.texcoord.x = u - floor(u);
	intersection.texcoord.y = v - floor(v);

	return true;
}

AABB Plane::getAABB() const
{
	return aabb;
}

void Plane::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	(void)scale;

	Matrix4x4 rotation = Matrix4x4::rotateXYZ(rotate);

	position += translate;
	normal = rotation.transformDirection(normal).normalized();
}
