// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>
#include <vector>

#include "Raytracing/Primitives/Triangle.h"
#include "Raytracing/Primitives/PrimitiveGroup.h"
#include "Raytracing/Textures/ImageTexture.h"
#include "Raytracing/Material.h"
#include "Math/Vector3.h"
#include "Math/EulerAngle.h"

namespace Raycer
{
	struct ModelLoaderInfo
	{
		std::string modelFilePath;
		Vector3 scale = Vector3(1.0, 1.0, 1.0);
		EulerAngle rotate = EulerAngle(0.0, 0.0, 0.0);
		Vector3 translate = Vector3(0.0, 0.0, 0.0);
		bool invisible = false;
		int idStartOffset = 1000000;
		int defaultMaterialId = 0;
	};

	struct ModelLoaderResult
	{
		std::vector<Triangle> triangles;
		std::vector<Material> materials;
		std::vector<ImageTexture> textures;
		std::vector<PrimitiveGroup> groups;
		PrimitiveGroup all;
	};
}
