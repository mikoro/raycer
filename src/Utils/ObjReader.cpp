// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <fstream>
#include <sstream>
#include <algorithm>

#include "Utils/ObjReader.h"
#include "App.h"
#include "Utils/Log.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Raytracing/Primitives/Triangle.h"

using namespace Raycer;

std::vector<Triangle> ObjReader::readFile(const std::string& fileName)
{
	Log& log = App::getLog();

	log.logInfo("Parsing obj file %s", fileName);

	std::ifstream file(fileName);
	std::string line;
	std::string part;

	if (!file.good())
		throw std::runtime_error("Could not open the obj file");

	std::vector<Vector3> vertices;
	std::vector<Vector2> texcoords;
	std::vector<Vector3> normals;
	std::vector<Triangle> triangles;

	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		ss >> part;

		// vertex
		if (part == "v")
		{
			Vector3 vertex;

			ss >> vertex.x;
			ss >> vertex.y;
			ss >> vertex.z;

			vertices.push_back(vertex);
			continue;
		}

		// texcoord
		if (part == "vt")
		{
			Vector2 texcoord;

			ss >> texcoord.x;
			ss >> texcoord.y;

			texcoords.push_back(texcoord);
			continue;
		}

		// normal
		if (part == "vn")
		{
			Vector3 normal;

			ss >> normal.x;
			ss >> normal.y;
			ss >> normal.z;

			normals.push_back(normal);
			continue;
		}

		// face (convex polygon)
		if (part == "f")
		{
			processFace(ss, vertices, texcoords, normals, triangles);
			continue;
		}
	}

	return triangles;
}

void ObjReader::processFace(std::istringstream& ss, std::vector<Vector3>& vertices, std::vector<Vector2>& texcoords, std::vector<Vector3>& normals, std::vector<Triangle>& triangles)
{
	std::vector<int> vertexIndices;
	std::vector<int> texcoordIndices;
	std::vector<int> normalIndices;

	std::string part;
	ss >> part;

	// determine what indices are available from the slash count
	int slashCount = (int)std::count(part.begin(), part.end(), '/');
	bool doubleSlash = (part.find("//") != std::string::npos);
	bool hasTexcoords = (slashCount > 0 && !doubleSlash);
	bool hasNormals = (slashCount > 1);

	do
	{
		// stringstream likes spaces
		std::replace(part.begin(), part.end(), '/', ' ');
		std::istringstream ssp(part);

		int vertexIndex;
		ssp >> vertexIndex;
		vertexIndex--;

		if (vertexIndex < 0 || vertexIndex >= (int)vertices.size())
			throw std::runtime_error("Vertex index was out of bounds");

		vertexIndices.push_back(vertexIndex);

		if (hasTexcoords)
		{
			int texcoordIndex;
			ssp >> texcoordIndex;
			texcoordIndex--;

			if (texcoordIndex < 0 || texcoordIndex >= (int)texcoords.size())
				throw std::runtime_error("Texcoord index was out of bounds");

			texcoordIndices.push_back(texcoordIndex);
		}

		if (hasNormals)
		{
			int normalIndex;
			ssp >> normalIndex;
			normalIndex--;

			if (normalIndex < 0 || normalIndex >= (int)normals.size())
				throw std::runtime_error("Normal index was out of bounds");

			normalIndices.push_back(normalIndex);
		}

	} while (ss >> part);

	if (vertexIndices.size() < 3)
		throw std::runtime_error("Too few vertices in a face");

	// triangulate
	for (int i = 2; i < vertexIndices.size(); ++i)
	{
		Triangle triangle;

		triangle.vertices[0] = vertices[vertexIndices[0]];
		triangle.vertices[1] = vertices[vertexIndices[i - 1]];
		triangle.vertices[2] = vertices[vertexIndices[i]];

		if (hasTexcoords)
		{
			triangle.texcoords[0] = texcoords[texcoordIndices[0]];
			triangle.texcoords[1] = texcoords[texcoordIndices[i - 1]];
			triangle.texcoords[2] = texcoords[texcoordIndices[i]];
		}

		if (hasNormals)
		{
			triangle.normals[0] = normals[normalIndices[0]];
			triangle.normals[1] = normals[normalIndices[i - 1]];
			triangle.normals[2] = normals[normalIndices[i]];
		}
		else // calculate normals manually
		{
			Vector3 v0tov1 = triangle.vertices[1] - triangle.vertices[0];
			Vector3 v0tov2 = triangle.vertices[2] - triangle.vertices[0];
			Vector3 normal = v0tov1.cross(v0tov2).normalized();

			triangle.normals[0] = triangle.normals[1] = triangle.normals[2] = normal;
		}

		triangles.push_back(triangle);
	}
}
