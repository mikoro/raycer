// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>

#include "Raytracing/Primitives/Mesh.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Utils/PlyReader.h"
#include "Utils/StringUtils.h"
#include "Math/Matrix4x4.h"

using namespace Raycer;

void Mesh::initialize()
{
	if (meshFilePath.length() > 0)
	{
// 		if (StringUtils::endsWith(meshFilePath, "obj"))
// 			triangles = ObjReader::getTriangles(meshFilePath);
// 		else if (StringUtils::endsWith(meshFilePath, "ply"))
// 			triangles = PlyReader::readFile(meshFilePath);
// 		else
// 			throw std::runtime_error("Unknown mesh file format");
	}
	
	Matrix4x4 scaling = Matrix4x4::scale(scale);
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(orientation);
	Matrix4x4 translation = Matrix4x4::translate(position);
	Matrix4x4 transformation = translation * rotation * scaling;
	Matrix4x4 transformationInvT = transformation.inverted().transposed();

	std::vector<Primitive*> primitives;

	for (Triangle& triangle : triangles)
	{
		triangle.vertices[0] = transformation.transformPosition(triangle.vertices[0]);
		triangle.vertices[1] = transformation.transformPosition(triangle.vertices[1]);
		triangle.vertices[2] = transformation.transformPosition(triangle.vertices[2]);

		triangle.normals[0] = transformationInvT.transformDirection(triangle.normals[0]).normalized();
		triangle.normals[1] = transformationInvT.transformDirection(triangle.normals[1]).normalized();
		triangle.normals[2] = transformationInvT.transformDirection(triangle.normals[2]).normalized();

		triangle.materialId = materialId;
		triangle.material = material;

		triangle.initialize();

		aabb.expand(triangle.getAABB());

		if (enableBVH)
			primitives.push_back(&triangle);
	}

	if (enableBVH)
		bvh.build(primitives, bvhBuildInfo);
}

bool Mesh::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	if (enableBVH)
		return bvh.intersect(ray, intersection, intersections);
	else
	{
		bool wasFound = false;

		for (Triangle& triangle : triangles)
		{
			if (triangle.intersect(ray, intersection, intersections))
				wasFound = true;
		}

		return wasFound;
	}
}

AABB Mesh::getAABB() const
{
	if (enableBVH)
		return bvh.getAABB();
	else
		return aabb;
}

void Mesh::transform(const Vector3& scale_, const EulerAngle& rotate_, const Vector3& translate_)
{
	if (enableBVH)
		bvh.transform(scale_, rotate_, translate_);
	else
	{
		for (Triangle& triangle : triangles)
			triangle.transform(scale_, rotate_, translate_);
	}
}
