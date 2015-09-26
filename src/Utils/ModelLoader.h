// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>
#include <vector>

#include "cereal/cereal.hpp"

#include "Raytracing/Primitives/Triangle.h"
#include "Raytracing/Primitives/PrimitiveGroup.h"
#include "Raytracing/Primitives/Instance.h"
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
		Material baseMaterial;
		bool enableGroups = false;
		bool enableGroupsInstances = false;
		bool enableCombinedGroup = true;
		bool enableCombinedGroupInstance = true;
		bool invisibleTriangles = true;
		bool invisibleGroups = true;
		bool invisibleCombinedGroup = true;
		bool ignoreMaterials = false;
		int defaultMaterialId = 0;
		int idStartOffset = 1000000;
		int combinedGroupId = 0;
		int combinedGroupInstanceId = 0;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(modelFilePath),
				CEREAL_NVP(scale),
				CEREAL_NVP(rotate),
				CEREAL_NVP(translate),
				CEREAL_NVP(baseMaterial),
				CEREAL_NVP(invisibleTriangles),
				CEREAL_NVP(ignoreMaterials),
				CEREAL_NVP(enableGroups),
				CEREAL_NVP(enableGroupsInstances),
				CEREAL_NVP(enableCombinedGroup),
				CEREAL_NVP(enableCombinedGroupInstance),
				CEREAL_NVP(defaultMaterialId),
				CEREAL_NVP(idStartOffset),
				CEREAL_NVP(combinedGroupId),
				CEREAL_NVP(combinedGroupInstanceId));
		}
	};

	struct ModelLoaderResult
	{
		std::vector<Triangle> triangles;
		std::vector<Material> materials;
		std::vector<ImageTexture> textures;
		std::vector<PrimitiveGroup> groups;
		std::vector<Instance> instances;
	};
}
