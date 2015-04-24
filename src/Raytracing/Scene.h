// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>
#include <map>

#include "Raytracing/Camera.h"
#include "Raytracing/Fog.h"
#include "Raytracing/Texture.h"
#include "Raytracing/ColorTexture.h"
#include "Raytracing/CheckerTexture.h"
#include "Raytracing/ImageTexture.h"
#include "Raytracing/WoodTexture.h"
#include "Raytracing/MarbleTexture.h"
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

		double gamma = 1.0 / 2.2;
		int multisamples = 1;
		int maxReflections = 0;

		Camera camera;
		Fog fog;

		std::vector<ColorTexture> colorTextures;
		std::vector<CheckerTexture> checkerTextures;
		std::vector<ImageTexture> imageTextures;
		std::vector<WoodTexture> woodTextures;
		std::vector<MarbleTexture> marbleTextures;
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
			ar(CEREAL_NVP(gamma),
				CEREAL_NVP(multisamples),
				CEREAL_NVP(maxReflections),
				CEREAL_NVP(camera),
				CEREAL_NVP(fog),
				CEREAL_NVP(colorTextures),
				CEREAL_NVP(checkerTextures),
				CEREAL_NVP(imageTextures),
				CEREAL_NVP(woodTextures),
				CEREAL_NVP(marbleTextures),
				CEREAL_NVP(materials),
				CEREAL_NVP(planes),
				CEREAL_NVP(spheres),
				CEREAL_NVP(meshes),
				CEREAL_NVP(lights));
		}
	};
}
