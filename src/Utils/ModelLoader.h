// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

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
	/*
	TRIANGLES: only generate visible triangles
	GROUPS: generate invisible triangles, generate invisible primitive groups based on groups in model file, generate visible instances to primitive groups (generated IDs)
	COMBINED_GROUP: generate invisible triangles, generate one invisible primitive group to all triangles, generate one visible instance to the primitive group (preset IDs)
	*/
	enum class ModelLoaderPreset { TRIANGLES, GROUPS, COMBINED_GROUP };

	struct ModelLoaderInfo
	{
		std::string modelFilePath;
		Vector3 scale = Vector3(1.0, 1.0, 1.0);
		EulerAngle rotate = EulerAngle(0.0, 0.0, 0.0);
		Vector3 translate = Vector3(0.0, 0.0, 0.0);
		Material baseMaterial;
		bool enableGroups = false;
		bool enableGroupsInstances = false;
		bool enableCombinedGroup = false;
		bool enableCombinedGroupInstance = false;
		bool invisibleTriangles = false;
		bool invisibleGroups = false;
		bool invisibleCombinedGroup = false;
		bool ignoreMaterials = false;
		unsigned defaultMaterialId = 0;
		unsigned idStartOffset = 0;
		unsigned combinedGroupId = 0;
		unsigned combinedGroupInstanceId = 0;

		ModelLoaderInfo() {}
		ModelLoaderInfo(ModelLoaderPreset preset)
		{
			if (preset == ModelLoaderPreset::GROUPS)
			{
				enableGroups = true;
				enableGroupsInstances = true;
				invisibleGroups = true;
				invisibleTriangles = true;
			}
			else if (preset == ModelLoaderPreset::COMBINED_GROUP)
			{
				enableCombinedGroup = true;
				enableCombinedGroupInstance = true;
				invisibleCombinedGroup = true;
				invisibleTriangles = true;
			}
		}

		template <class Archive>
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

	class ModelLoader
	{
	public:

		static ModelLoaderResult readObjFile(const ModelLoaderInfo& info);
		static ModelLoaderResult readPlyFile(const ModelLoaderInfo& info);
	};
}
