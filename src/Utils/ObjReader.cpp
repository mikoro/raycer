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
	bool isValid = true;

	if (!file.good())
		throw std::runtime_error("Could not open the obj file");

	std::vector<Vector3> vertices;
	std::vector<Vector2> texcoords;
	std::vector<Vector3> normals;
	std::vector<Triangle> triangles;

	std::vector<int> vertexIndices;
	std::vector<int> texcoordIndices;
	std::vector<int> normalIndices;

	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		ss >> part;

		if (part == "v")
		{
			Vector3 vertex;

			ss >> vertex.x;
			ss >> vertex.y;
			ss >> vertex.z;

			vertices.push_back(vertex);
			continue;
		}

		if (part == "vt")
		{
			Vector2 texcoord;

			ss >> texcoord.x;
			ss >> texcoord.y;

			texcoords.push_back(texcoord);
			continue;
		}

		if (part == "vn")
		{
			Vector3 normal;

			ss >> normal.x;
			ss >> normal.y;
			ss >> normal.z;

			normals.push_back(normal);
			continue;
		}

		if (part == "f")
		{
			ss >> part;

			int slashCount = std::count(part.begin(), part.end(), '/');
			bool doubleSlash = (part.find("//") != std::string::npos);
			bool hasTexcoord = (slashCount > 0);
			bool hasNormal = (slashCount > 1);

			vertexIndices.clear();
			texcoordIndices.clear();
			normalIndices.clear();

			do
			{
				std::replace(part.begin(), part.end(), '/', ' ');
				
				int vertexIndex, texcoordIndex, normalIndex;
				std::istringstream ssp(part);
				ssp >> vertexIndex;
				ssp >> texcoordIndex;
				ssp >> normalIndex;

			} while (ss >> part);

			// a face needs at least three vertices
// 			if (parts.size() < 3)
// 			{
// 				isValid = false;
// 				continue;
// 			}

			continue;
		}
	}

	return triangles;
}
