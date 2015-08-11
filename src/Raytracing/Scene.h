// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <map>
#include <string>
#include <vector>

#include "Raytracing/Camera.h"
#include "Raytracing/Textures/ColorTexture.h"
#include "Raytracing/Textures/CheckerTexture.h"
#include "Raytracing/Textures/ImageTexture.h"
#include "Raytracing/Textures/PerlinNoiseTexture.h"
#include "Raytracing/Textures/ValueNoiseTexture.h"
#include "Raytracing/Textures/CellNoiseTexture.h"
#include "Raytracing/Textures/MarbleTexture.h"
#include "Raytracing/Textures/WoodTexture.h"
#include "Raytracing/Textures/FireTexture.h"
#include "Raytracing/Textures/AtmosphereTexture.h"
#include "Raytracing/Textures/VoronoiTexture.h"
#include "Raytracing/Material.h"
#include "Raytracing/Lights.h"
#include "Raytracing/Primitives/Plane.h"
#include "Raytracing/Primitives/Sphere.h"
#include "Raytracing/Primitives/Box.h"
#include "Raytracing/Primitives/Triangle.h"
#include "Raytracing/Primitives/Mesh.h"
#include "Raytracing/Primitives/BVH.h"
#include "Math/Color.h"

namespace Raycer
{
	enum class MultisampleType { NONE, RANDOM, REGULAR_GRID, JITTER, CORRELATED_MULTI_JITTER };
	enum class ToneMapType { NONE, GAMMA, REINHARD };
	
	class Primitive;
	class Texture;
	
	class Scene
	{
	public:

		Scene();

		void loadFromFile(const std::string& fileName);
		void loadFromJsonString(const std::string& text);
		void loadFromXmlString(const std::string& text);
		void saveToFile(const std::string& fileName) const;
		std::string saveToJsonString() const;
		std::string saveToXmlString() const;

		void initialize();

		struct Tracer
		{
			int maxIterations = 3;
			double rayStartOffset = 0.000001;
		} tracer;

		struct Multisampler
		{
			MultisampleType type = MultisampleType::NONE;
			int multisamples = 3;
		} multisampler;
		
		struct ToneMapper
		{
			ToneMapType type = ToneMapType::GAMMA;
			double gamma = 1.0 / 2.2;
		} toneMapper;

		Camera camera;

		struct Fog
		{
			bool enabled = false;
			Color color;
			double distance = 0.0;
			double steepness = 1.0;
			bool heightDispersion = false;
			double height = 0.0;
			double heightSteepness = 1.0;
		} fog;
		
		struct Textures
		{
			std::vector<ColorTexture> colorTextures;
			std::vector<CheckerTexture> checkerTextures;
			std::vector<ImageTexture> imageTextures;
			std::vector<PerlinNoiseTexture> perlinNoiseTextures;
			std::vector<ValueNoiseTexture> valueNoiseTextures;
			std::vector<CellNoiseTexture> cellNoiseTextures;
			std::vector<MarbleTexture> marbleTextures;
			std::vector<WoodTexture> woodTextures;
			std::vector<FireTexture> fireTextures;
			std::vector<AtmosphereTexture> atmosphereTextures;
			std::vector<VoronoiTexture> voronoiTextures;
		} textures;

		std::vector<Material> materials;

		struct Lights
		{
			AmbientLight ambientLight;
			std::vector<DirectionalLight> directionalLights;
			std::vector<PointLight> pointLights;
			std::vector<SpotLight> spotLights;
		} lights;

		struct Primitives
		{
			std::vector<Plane> planes;
			std::vector<Sphere> spheres;
			std::vector<Box> boxes;
			std::vector<Triangle> triangles;
			std::vector<Mesh> meshes;
			std::vector<BVH> bvhs;
			std::vector<Primitive*> all;
		} primitives;

		std::map<int, Texture*> texturesMap;
		std::map<int, Material*> materialsMap;
	};
}
