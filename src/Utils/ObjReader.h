// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include "Raytracing/Primitives/Mesh.h"
#include "Raytracing/Material.h"
#include "Raytracing/Textures/ColorTexture.h"
#include "Raytracing/Textures/ImageTexture.h"

namespace Raycer
{
	class Vector3;
	class Vector2;
	class Triangle;

	struct ObjReaderResult
	{
		std::vector<Mesh> meshes;
		std::vector<Material> materials;
		std::vector<ColorTexture> colorTextures;
		std::vector<ImageTexture> imageTextures;
	};

	class ObjReader
	{
	public:

		static std::vector<Triangle> getTriangles(const std::string& objFileName);
		static ObjReaderResult getMeshes(const std::string& objFileName, int idStartOffset = 1000000);

	private:

		static void processMaterialFile(const std::string& mtlFileName, ObjReaderResult& result, std::map<std::string, int>& materialMap, int& currentId);
		static void processFace(std::istringstream& ss, std::vector<Vector3>& vertices, std::vector<Vector2>& texcoords, std::vector<Vector3>& normals, std::vector<Triangle>& triangles);
	};
}
