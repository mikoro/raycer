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

void Raycer::Instance::initialize(const Scene& scene)
{
	(void)scene;

	Vector3 position = primitive->getAABB().getCenter();

	Matrix4x4 scaling = Matrix4x4::scale(scale);
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(rotate);
	Matrix4x4 translation1 = Matrix4x4::translate(-position);
	Matrix4x4 translation2 = Matrix4x4::translate(position + translate);

	cachedTransformation = translation2 * rotation * scaling * translation1;
	cachedTransformationInv = cachedTransformation.inverted();
	cachedTransformationInvT = cachedTransformationInv.transposed();

	aabb = primitive->getAABB().transformed(scale, rotate, translate);
}

bool Instance::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	if (isTimeVariant)
	{
		Vector3 position = primitive->getAABB().getCenter();
		Vector3 newScale = scale + ray.time * scaleInTime;
		EulerAngle newRotate = rotate + ray.time * rotateInTime;
		Vector3 newTranslate = translate + ray.time * translateInTime;

		Matrix4x4 scaling = Matrix4x4::scale(newScale);
		Matrix4x4 rotation = Matrix4x4::rotateXYZ(newRotate);
		Matrix4x4 translation1 = Matrix4x4::translate(-position);
		Matrix4x4 translation2 = Matrix4x4::translate(position + newTranslate);

		Matrix4x4 newTransformation = translation2 * rotation * scaling * translation1;
		Matrix4x4 newTransformationInv = newTransformation.inverted();
		Matrix4x4 newTransformationInvT = newTransformationInv.transposed();

		return internalIntersect(ray, intersection, intersections, newTransformation, newTransformationInv, newTransformationInvT);
	}
	else
		return internalIntersect(ray, intersection, intersections, cachedTransformation, cachedTransformationInv, cachedTransformationInvT);
}

bool Instance::internalIntersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections, const Matrix4x4& transformation, const Matrix4x4& transformationInv, const Matrix4x4& transformationInvT)
{
	Ray instanceRay;
	Intersection instanceIntersection;
	std::vector<Intersection> instanceIntersections;

	instanceRay.origin = transformationInv.transformPosition(ray.origin);
	instanceRay.direction = transformationInv.transformDirection(ray.direction).normalized();
	instanceRay.time = ray.time;
	instanceRay.fastOcclusion = ray.fastOcclusion;
	instanceRay.isShadowRay = ray.isShadowRay;
	instanceRay.update();

	bool wasFound = primitive->intersect(instanceRay, instanceIntersection, instanceIntersections);

	for (Intersection& tempIntersection : instanceIntersections)
	{
		Vector3 position = transformation.transformPosition(tempIntersection.position);

		tempIntersection.distance = (position - ray.origin).length();
		tempIntersection.position = position;
		tempIntersection.normal = transformationInvT.transformDirection(tempIntersection.normal).normalized();
		tempIntersection.onb = tempIntersection.onb.transformed(transformationInvT);

		intersections.push_back(tempIntersection);
	}

	if (wasFound)
	{
		Vector3 position = transformation.transformPosition(instanceIntersection.position);
		double t = (position - ray.origin).length();

		if (t < ray.minDistance || t > ray.maxDistance)
			return false;

		if (t > intersection.distance)
			return false;

		intersection.wasFound = true;
		intersection.distance = t;
		intersection.primitive = changePrimitive ? this : instanceIntersection.primitive;
		intersection.instancePrimitive = this;
		intersection.position = position;
		intersection.normal = transformationInvT.transformDirection(instanceIntersection.normal).normalized();
		intersection.onb = instanceIntersection.onb.transformed(transformationInvT);
		intersection.texcoord = instanceIntersection.texcoord;

		return true;
	}

	return false;
}

AABB Instance::getAABB() const
{
	return aabb;
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

	cachedTransformation = translation2 * rotation * scaling * translation1;
	cachedTransformationInv = cachedTransformation.inverted();
	cachedTransformationInvT = cachedTransformationInv.transposed();

	aabb = primitive->getAABB().transformed(scale, rotate, translate);
}
