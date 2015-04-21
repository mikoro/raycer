// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>
#include <map>

#include "Raytracing/Camera.h"
#include "Raytracing/Texture.h"
#include "Raytracing/ColorTexture.h"
#include "Raytracing/CheckerboardTexture.h"
#include "Raytracing/ImageTexture.h"
#include "Raytracing/Material.h"
#include "Raytracing/Primitive.h"
#include "Raytracing/Plane.h"
#include "Raytracing/Sphere.h"
#include "Raytracing/Mesh.h"
#include "Raytracing/Light.h"
#include "Math/Color.h"

namespace Raycer
{
	class Scene
	{
	public:

		void loadFromString(const std::string& text);
		void loadFromFile(const std::string& fileName);
		void saveAs(const std::string& fileName) const;
		void initialize();
		static Scene createTestScene();

		Camera camera;

		int maxReflections = 0;
		bool fogEnabled = false;
		double fogDistance = 0.0;
		double fogSteepness = 0.0;
		Color fogColor = Color(0.0, 0.0, 0.0);

		std::vector<ColorTexture> colorTextures;
		std::vector<CheckerboardTexture> checkerboardTextures;
		std::vector<ImageTexture> imageTextures;
		std::vector<Material> materials;
		std::vector<Plane> planes;
		std::vector<Sphere> spheres;
		std::vector<Mesh> meshes;
		std::vector<Light> lights;
		
		std::vector<Texture*> textureList;
		std::map<int, Texture*> textureMap;
		std::map<int, Material*> materialMap;
		std::vector<Primitive*> primitiveList;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(camera),
				CEREAL_NVP(maxReflections),
				CEREAL_NVP(fogEnabled),
				CEREAL_NVP(fogDistance),
				CEREAL_NVP(fogSteepness),
				CEREAL_NVP(fogColor),
				CEREAL_NVP(colorTextures),
				CEREAL_NVP(checkerboardTextures),
				CEREAL_NVP(imageTextures),
				CEREAL_NVP(materials),
				CEREAL_NVP(planes),
				CEREAL_NVP(spheres),
				CEREAL_NVP(meshes),
				CEREAL_NVP(lights));
		}
	};
}
