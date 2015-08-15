// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

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
#include "Raytracing/Primitives/FlatBVH.h"
#include "Math/Color.h"

namespace Raycer
{
	enum class MultisampleType { CMJ, RANDOM, REGULAR, JITTER };
	enum class ToneMapType { GAMMA, REINHARD };
	
	class Primitive;
	class Texture;
	
	class Scene
	{
	public:

		Scene();

		static Scene createTestScene(int number);
		static Scene loadFromFile(const std::string& fileName);
		static Scene loadFromJsonString(const std::string& text);
		static Scene loadFromXmlString(const std::string& text);

		void saveToFile(const std::string& fileName) const;
		std::string saveToJsonString() const;
		std::string saveToXmlString() const;

		void initialize();

		static Scene createTestScene1();
		static Scene createTestScene2();
		static Scene createTestScene3();
		static Scene createTestScene4();
		static Scene createTestScene5();
		static Scene createTestScene6();
		static Scene createTestScene7();
		static Scene createTestScene8();
		static Scene createTestScene9();
		static Scene createTestScene10();

		Camera camera;

		struct Misc
		{
			Color backgroundColor = Color::BLACK;
		} misc;

		struct Fog
		{
			bool enabled = false;
			Color color = Color::WHITE;
			double distance = 100.0;
			double steepness = 1.0;
			bool heightDispersion = false;
			double height = 100.0;
			double heightSteepness = 1.0;
		} fog;

		struct Raytracing
		{
			int maxIterations = 3;
			double startOffset = 0.000001;
		} raytracing;

		struct Multisampling
		{
			bool enabled = false;
			MultisampleType type = MultisampleType::CMJ;
			int samples = 3;
		} multisampling;
		
		struct ToneMapping
		{
			bool enabled = true;
			ToneMapType type = ToneMapType::GAMMA;
			double gamma = 1.0 / 2.2;
		} toneMapping;

		struct RootBVH
		{
			bool enabled = true;
			BVHBuildInfo buildInfo;
		} rootBVH;

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
			std::vector<Primitive*> all;
			FlatBVH rootBVH;
		} primitives;
	};
}
