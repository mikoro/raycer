// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <iosfwd>
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

		static void processMaterialFile(const std::string& objFileDirectory, const std::string& mtlFilePath, ModelLoaderResult& result, std::map<std::string, int>& materialsMap, int& currentId);
		static void processFace(std::istringstream& ss, std::vector<Vector3>& vertices, std::vector<Vector3>& normals, std::vector<Vector2>& texcoords, ModelLoaderResult& result, int& currentId, int currentMaterialId, bool invisible);
	};
}
