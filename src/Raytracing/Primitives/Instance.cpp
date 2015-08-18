// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Raytracing/Primitives/Instance.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"

using namespace Raycer;

void Instance::initialize()
{
	Vector3 position = primitive->getAABB().center;

	Matrix4x4 scaling = Matrix4x4::scale(scale);
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(rotate);
	Matrix4x4 translation1 = Matrix4x4::translate(-position);
	Matrix4x4 translation2 = Matrix4x4::translate(position + translate);

	transformation = translation2 * rotation * scaling * translation1;
	transformationInv = transformation.inverted();
	transformationInvT = transformationInv.transposed();
}

bool Instance::intersect(const Ray& ray, Intersection& intersection)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	Ray instanceRay = ray;
	Intersection instanceIntersection = intersection;

	instanceRay.origin = transformationInv.transformPosition(ray.origin);
	instanceRay.direction = transformationInv.transformDirection(ray.direction).normalized();
	instanceRay.update();

	if (primitive->intersect(instanceRay, instanceIntersection))
	{
		Vector3 position = transformation.transformPosition(instanceIntersection.position);
		
		double t = (position - ray.origin).length();

		if (t < ray.tmin || t > ray.tmax)
			return false;

		if (t > intersection.distance)
			return false;

		intersection.wasFound = true;
		intersection.distance = t;
		intersection.primitive = this;
		intersection.position = position;
		intersection.normal = transformationInvT.transformDirection(instanceIntersection.normal).normalized();
		intersection.texcoord = instanceIntersection.texcoord;
		intersection.onb = ONB::fromNormal(intersection.normal);

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

	Vector3 position = primitive->getAABB().center;

	Matrix4x4 scaling = Matrix4x4::scale(scale);
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(rotate);
	Matrix4x4 translation1 = Matrix4x4::translate(-position);
	Matrix4x4 translation2 = Matrix4x4::translate(position + translate);

	transformation = translation2 * rotation * scaling * translation1;
	transformationInv = transformation.inverted();
	transformationInvT = transformationInv.transposed();
}
