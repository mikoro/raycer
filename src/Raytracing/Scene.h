// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>
#include <map>

#include "Raytracing/Camera.h"
#include "Raytracing/Fog.h"
#include "Raytracing/Textures/Texture.h"
#include "Raytracing/Textures/ColorTexture.h"
#include "Raytracing/Textures/CheckerTexture.h"
#include "Raytracing/Textures/ImageTexture.h"
#include "Raytracing/Textures/PerlinNoiseTexture.h"
#include "Raytracing/Textures/CellNoiseTexture.h"
#include "Raytracing/Textures/MarbleTexture.h"
#include "Raytracing/Textures/WoodTexture.h"
#include "Raytracing/Textures/FireTexture.h"
#include "Raytracing/Material.h"
#include "Raytracing/Lights.h"
#include "Raytracing/Primitives/Primitive.h"
#include "Raytracing/Primitives/Plane.h"
#include "Raytracing/Primitives/Sphere.h"
#include "Raytracing/Primitives/Mesh.h"

namespace Raycer
{
	enum class MultisampleType { NONE, UNIFORM, REGULAR, JITTER, POISSON };
	enum class ToneMapType { NONE, GAMMA, REINHARD };

	class Scene
	{
	public:

		void loadFromFile(const std::string& fileName);
		void loadFromJsonString(const std::string& text);
		void loadFromXmlString(const std::string& text);
		void saveToFile(const std::string& fileName) const;
		std::string saveToJsonString() const;
		std::string saveToXmlString() const;

		void initialize();
		void validate();

		static Scene createTestScene1();
		static Scene createTestScene2();

		struct Tracer
		{
			int maxIterations = 0;
			double airRefractiveIndex = 1.0;
			double rayOffset = 0.000001;
		} tracer;

		struct Multisampler
		{
			MultisampleType type = MultisampleType::NONE;
			int multisamples = 4;
		} multisampler;
		
		struct ToneMapper
		{
			ToneMapType type = ToneMapType::GAMMA;
			double gamma = 1.0 / 2.2;
		} toneMapper;

		Camera camera;
		Fog fog;
		
		struct Textures
		{
			std::vector<ColorTexture> colorTextures;
			std::vector<CheckerTexture> checkerTextures;
			std::vector<ImageTexture> imageTextures;
			std::vector<PerlinNoiseTexture> perlinNoiseTextures;
			std::vector<CellNoiseTexture> cellNoiseTextures;
			std::vector<MarbleTexture> marbleTextures;
			std::vector<WoodTexture> woodTextures;
			std::vector<FireTexture> fireTextures;
		} textures;

		std::vector<Material> materials;

		struct Lights
		{
			std::vector<AmbientLight> ambientLights;
			std::vector<DirectionalLight> directionalLights;
			std::vector<PointLight> pointLights;
			std::vector<SpotLight> spotLights;
		} lights;

		struct Primitives
		{
			std::vector<Plane> planes;
			std::vector<Sphere> spheres;
			std::vector<Mesh> meshes;
		} primitives;
		
		std::vector<Texture*> texturesList;
		std::map<int, Texture*> texturesMap;
		std::map<int, Material*> materialsMap;
		std::vector<Primitive*> primitivesList;
	};
}
