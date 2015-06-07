// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <exception>

#include "Raytracing/Primitives/Mesh.h"
#include "Raytracing/Ray.h"
#include "Utils/ObjReader.h"
#include "Utils/PlyReader.h"
#include "Utils/StringUtils.h"
#include "Math/Matrix4x4.h"

using namespace Raycer;

void Mesh::initialize()
{
	if (StringUtils::endsWith(meshFilePath, "obj"))
		triangles = ObjReader::readFile(meshFilePath);
	else if (StringUtils::endsWith(meshFilePath, "ply"))
		triangles = PlyReader::readFile(meshFilePath);
	else
		throw std::runtime_error("Unknown mesh file format");

	Matrix4x4 rotationX = Matrix4x4::rotateX(orientation.pitch);
	Matrix4x4 rotationY = Matrix4x4::rotateY(orientation.yaw);
	Matrix4x4 rotationZ = Matrix4x4::rotateZ(orientation.roll);
	Matrix4x4 rotation = rotationX * rotationY * rotationZ;
	Matrix4x4 scaling = Matrix4x4::scale(scale.x, scale.y, scale.z);
	Matrix4x4 translation = Matrix4x4::translate(position.x, position.y, position.z);
	Matrix4x4 transformation = translation * scaling * rotation;

	for (Triangle& triangle : triangles)
	{
		triangle.vertices[0] = transformation * triangle.vertices[0];
		triangle.vertices[1] = transformation * triangle.vertices[1];
		triangle.vertices[2] = transformation * triangle.vertices[2];

		// TODO: does not work with non-uniform scaling
		triangle.normals[0] = rotation * triangle.normals[0];
		triangle.normals[1] = rotation * triangle.normals[1];
		triangle.normals[2] = rotation * triangle.normals[2];

		triangle.normal = rotation * triangle.normal;
	}
}

void Mesh::intersect(Ray& ray) const
{
	for (const Triangle& triangle : triangles)
	{
		if (triangle.intersect(ray))
		{
			ray.intersection.materialId = materialId;
			ray.intersection.texcoord *= texcoordScale;
		}
	}
}
