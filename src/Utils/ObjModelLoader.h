// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <map>
#include <string>
#include <vector>

#include "Utils/ModelLoader.h"

namespace Raycer
{
	class ObjModelLoader
	{
	public:

		static ModelLoaderResult readFile(const ModelLoaderInfo& info);

	private:

		static void processMaterialFile(const std::string& objFileDirectory, const std::string& mtlFilePath, const ModelLoaderInfo& info, ModelLoaderResult& result, std::map<std::string, int>& materialsMap, int& currentId);
		static void processFace(const std::string& line, std::vector<Vector3>& vertices, std::vector<Vector3>& normals, std::vector<Vector2>& texcoords, const ModelLoaderInfo& info, ModelLoaderResult& result, PrimitiveGroup &combinedGroup, int& currentId, int currentMaterialId);
	};
}
