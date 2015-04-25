// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>
#include <map>

#include "cereal/cereal.hpp"

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
	enum class MultisampleType { NONE, UNIFORM, REGULAR, JITTER, POISSON };
	enum class ToneMapType { NONE, GAMMA, REINHARD };

	class Scene
	{
	public:

		void loadFromString(const std::string& text);
		void loadFromFile(const std::string& fileName);
		void saveAs(const std::string& fileName) const;
		void initialize();
		void validate();
		static Scene createTestScene();

		struct Tracer
		{
			int maxReflections = 0;

			template<class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(maxReflections));
			}
		} tracer;

		struct Multisampler
		{
			MultisampleType type = MultisampleType::NONE;
			int multisamples = 4;

			template<class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(type),
					CEREAL_NVP(multisamples));
			}
		} multisampler;
		
		struct ToneMapper
		{
			ToneMapType type = ToneMapType::GAMMA;
			double gamma = 1.0 / 2.2;

			template<class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(type),
					CEREAL_NVP(gamma));
			}
		} toneMapper;

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
		
		std::vector<Texture*> texturesList;
		std::map<int, Texture*> texturesMap;
		std::map<int, Material*> materialsMap;
		std::vector<Primitive*> primitivesList;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(tracer),
				CEREAL_NVP(multisampler),
				CEREAL_NVP(toneMapper),
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
