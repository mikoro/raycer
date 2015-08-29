// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Primitives/Instance.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Raytracing/ONB.h"
#include "Math/Vector2.h"

using namespace Raycer;

void Instance::initialize()
{
	Vector3 position = primitive->getAABB().getCenter();

	Matrix4x4 scaling = Matrix4x4::scale(scale);
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(rotate);
	Matrix4x4 translation1 = Matrix4x4::translate(-position);
	Matrix4x4 translation2 = Matrix4x4::translate(position + translate);

	transformation = translation2 * rotation * scaling * translation1;
	transformationInv = transformation.inverted();
	transformationInvT = transformationInv.transposed();
}

bool Instance::intersect(const Ray& ray, std::array<Intersection, 2>& intersections)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersections[0].wasFound)
		return true;

	Ray instanceRay = ray;
	std::array<Intersection, 2> instanceIntersections;
	instanceIntersections[0] = intersections[0];
	instanceIntersections[1] = intersections[1];

	instanceRay.origin = transformationInv.transformPosition(ray.origin);
	instanceRay.direction = transformationInv.transformDirection(ray.direction).normalized();
	instanceRay.update();

	if (primitive->intersect(instanceRay, instanceIntersections))
	{
		Vector3 position = transformation.transformPosition(instanceIntersections[0].position);

		double t = (position - ray.origin).length();

		if (t < ray.minDistance || t > ray.maxDistance)
			return false;

		if (t > intersections[0].distance)
			return false;

		intersections[0].wasFound = true;
		intersections[0].distance = t;
		intersections[0].primitive = this;
		intersections[0].position = position;
		intersections[0].normal = transformationInvT.transformDirection(instanceIntersections[0].normal).normalized();
		intersections[0].onb = instanceIntersections[0].onb.transformed(transformationInvT);
		intersections[0].texcoord = instanceIntersections[0].texcoord;

		return true;
	}

	return false;
}

AABB Instance::getAABB() const
{
	return primitive->getAABB().transformed(scale, rotate, translate);
}

void Instance::transform(const Vector3& scale_, const EulerAngle& rotate_, const Vector3& translate_)
{
	scale *= scale_;
	rotate += rotate_;
	translate += translate_;

	Vector3 position = primitive->getAABB().getCenter();

	Matrix4x4 scaling = Matrix4x4::scale(scale);
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(rotate);
	Matrix4x4 translation1 = Matrix4x4::translate(-position);
	Matrix4x4 translation2 = Matrix4x4::translate(position + translate);

	transformation = translation2 * rotation * scaling * translation1;
	transformationInv = transformation.inverted();
	transformationInvT = transformationInv.transposed();
}
