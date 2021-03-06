// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Primitives/Triangle.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Raytracing/Textures/Texture.h"
#include "Math/ONB.h"
#include "Math/Matrix4x4.h"

using namespace Raycer;

void Raycer::Triangle::initialize(const Scene& scene)
{
	(void)scene;

	Vector3 v0tov1 = vertices[1] - vertices[0];
	Vector3 v0tov2 = vertices[2] - vertices[0];
	Vector2 t0tot1 = texcoords[1] - texcoords[0];
	Vector2 t0tot2 = texcoords[2] - texcoords[0];

	normal = v0tov1.cross(v0tov2).normalized();

	double denominator = t0tot1.x * t0tot2.y - t0tot1.y * t0tot2.x;

	// tangent space aligned to texcoords
	if (std::abs(denominator) > std::numeric_limits<double>::epsilon())
	{
		double r = 1.0 / denominator;
		tangent = (v0tov1 * t0tot2.y - v0tov2 * t0tot1.y) * r;
		bitangent = (v0tov2 * t0tot1.x - v0tov1 * t0tot2.x) * r;
		tangent.normalize();
		bitangent.normalize();
	}
	else
	{
		tangent = normal.cross(Vector3::ALMOST_UP).normalized();
		bitangent = tangent.cross(normal).normalized();
	}

	aabb = AABB::createFromVertices(vertices[0], vertices[1], vertices[2]);
}

// Möller-Trumbore algorithm
// http://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
bool Triangle::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
{
	(void)intersections;

	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	Vector3 v0v1 = vertices[1] - vertices[0];
	Vector3 v0v2 = vertices[2] - vertices[0];

	Vector3 pvec = ray.direction.cross(v0v2);
	double determinant = v0v1.dot(pvec);

	// ray and triangle are parallel -> no intersection
	if (std::abs(determinant) < std::numeric_limits<double>::epsilon())
		return false;

	double invDeterminant = 1.0 / determinant;

	Vector3 tvec = ray.origin - vertices[0];
	double u = tvec.dot(pvec) * invDeterminant;

	if (u < 0.0 || u > 1.0)
		return false;

	Vector3 qvec = tvec.cross(v0v1);
	double v = ray.direction.dot(qvec) * invDeterminant;

	if (v < 0.0 || (u + v) > 1.0)
		return false;

	double t = v0v2.dot(qvec) * invDeterminant;

	if (t < 0.0)
		return false;

	if (t < ray.minDistance || t > ray.maxDistance)
		return false;

	if (t > intersection.distance)
		return false;

	double w = 1.0 - u - v;

	Vector2 texcoord = (w * texcoords[0] + u * texcoords[1] + v * texcoords[2]) * material->texcoordScale;
	Vector3 ip = ray.origin + (t * ray.direction);

	texcoord.x = texcoord.x - floor(texcoord.x);
	texcoord.y = texcoord.y - floor(texcoord.y);

	if (material->maskMapTexture != nullptr)
	{
		if (material->maskMapTexture->getValue(texcoord, ip) < 0.5)
			return false;
	}

	Vector3 finalNormal = material->normalInterpolation ? (w * normals[0] + u * normals[1] + v * normals[2]) : normal;

	intersection.wasFound = true;
	intersection.distance = t;
	intersection.primitive = this;
	intersection.position = ip;
	intersection.normal = material->invertNormal ? -finalNormal : finalNormal;
	intersection.onb = ONB(tangent, bitangent, intersection.normal);
	intersection.texcoord = texcoord;

	return true;
}

AABB Triangle::getAABB() const
{
	return aabb;
}

void Triangle::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	Vector3 center = (vertices[0] + vertices[1] + vertices[2]) / 3.0;

	Matrix4x4 scaling = Matrix4x4::scale(scale);
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(rotate);
	Matrix4x4 translation1 = Matrix4x4::translate(-center);
	Matrix4x4 translation2 = Matrix4x4::translate(translate + center);
	Matrix4x4 transformation = translation2 * rotation * scaling * translation1;
	Matrix4x4 transformationInvT = transformation.inverted().transposed();

	vertices[0] = transformation.transformPosition(vertices[0]);
	vertices[1] = transformation.transformPosition(vertices[1]);
	vertices[2] = transformation.transformPosition(vertices[2]);

	normals[0] = transformationInvT.transformDirection(normals[0]).normalized();
	normals[1] = transformationInvT.transformDirection(normals[1]).normalized();
	normals[2] = transformationInvT.transformDirection(normals[2]).normalized();

	normal = transformationInvT.transformDirection(normal).normalized();
	tangent = transformationInvT.transformDirection(tangent).normalized();
	bitangent = transformationInvT.transformDirection(bitangent).normalized();

	aabb = AABB::createFromVertices(vertices[0], vertices[1], vertices[2]);
}
