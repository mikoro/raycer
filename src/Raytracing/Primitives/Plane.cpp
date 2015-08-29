// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>
#include <limits>

#include "Raytracing/Primitives/Plane.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Raytracing/ONB.h"
#include "Math/Vector2.h"
#include "Math/Matrix4x4.h"

using namespace Raycer;

void Plane::initialize()
{
	uAxis = Vector3(normal.y, normal.z, -normal.x).normalized();
	vAxis = uAxis.cross(normal).normalized();
}

bool Plane::intersect(const Ray& ray, std::array<Intersection, 2>& intersections)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersections[0].wasFound)
		return true;

	double denominator = ray.direction.dot(normal);

	// ray and plane are parallel -> no intersection
	if (std::abs(denominator) < std::numeric_limits<double>::epsilon())
		return false;

	double t = (position - ray.origin).dot(normal) / denominator;

	if (t < 0.0 || t < ray.minDistance || t > ray.maxDistance)
		return false;

	// another intersection is closer
	if (t > intersections[0].distance)
		return false;

	// intersection position
	Vector3 ip = ray.origin + (t * ray.direction);

	intersections[0].wasFound = true;
	intersections[0].distance = t;
	intersections[0].primitive = this;
	intersections[0].position = ip;
	intersections[0].normal = normal;
	intersections[0].onb = ONB::fromNormal(normal);

	// texture coordinate calculation
	double u = uAxis.dot(ip) / material->texcoordScale.x;
	double v = vAxis.dot(ip) / material->texcoordScale.y;

	intersections[0].texcoord.x = std::abs(u - floor(u));
	intersections[0].texcoord.y = std::abs(v - floor(v));

	return true;
}

AABB Plane::getAABB() const
{
	// TODO: create bounds for the plane aabb
	return AABB::createFromMinMax(Vector3(-1000.0, -1000.0, -1000.0), Vector3(1000.0, 1000.0, 1000.0));
}

void Plane::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	(void)scale;

	Matrix4x4 rotation = Matrix4x4::rotateXYZ(rotate);

	position += translate;
	normal = rotation.transformDirection(normal).normalized();
}
