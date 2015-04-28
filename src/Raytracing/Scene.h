// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>
#include <map>

#include "Raytracing/Camera.h"
#include "Raytracing/Fog.h"
#include "Raytracing/Textures.h"
#include "Raytracing/Material.h"
#include "Raytracing/Lights.h"
#include "Raytracing/Primitive.h"
#include "Raytracing/Plane.h"
#include "Raytracing/Sphere.h"
#include "Raytracing/Mesh.h"

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
