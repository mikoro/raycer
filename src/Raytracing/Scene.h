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
#include "Raytracing/Primitives/Cylinder.h"
#include "Raytracing/Primitives/Torus.h"
#include "Raytracing/Primitives/Instance.h"
#include "Raytracing/Primitives/FlatBVH.h"
#include "Raytracing/Primitives/CSG.h"
#include "Raytracing/Primitives/BlinnBlob.h"
#include "Raytracing/Primitives/PrimitiveGroup.h"
#include "Rendering/Samplers/Sampler.h"
#include "Rendering/Filters/Filter.h"
#include "Rendering/ToneMappers/ToneMapper.h"
#include "Math/Color.h"
#include "Math/Vector3.h"
#include "Utils/ModelLoader.h"

namespace Raycer
{
	class Primitive;

	class Scene
	{
	public:

		Scene();

		static Scene createTestScene(int number);
		static Scene loadFromFile(const std::string& fileName);
		static Scene loadFromJsonString(const std::string& text);
		static Scene loadFromXmlString(const std::string& text);

		void saveToFile(const std::string& fileName) const;
		std::string getJsonString() const;
		std::string getXmlString() const;

		void addModel(const ModelLoaderResult& result);
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
		static Scene createTestScene11();
		static Scene createTestScene12();
		static Scene createTestScene13();
		static Scene createTestScene14();
		static Scene createTestScene15();
		static Scene createTestScene16();
		static Scene createTestScene17();
		static Scene createTestScene18();

		static const int TEST_SCENE_COUNT = 18;

		Camera camera;

		struct Raytracer
		{
			int maxRayIterations = 3;
			double rayStartOffset = 0.000001;
			Color backgroundColor = Color(0.0, 0.0, 0.0);
			Color offLensColor = Color(0.0, 0.0, 0.0);
			int multiSamples = 0;
			int timeSamples = 0;
			int cameraSamples = 0;
			SamplerType multiSamplerType = SamplerType::CMJ;
			FilterType multiSamplerFilterType = FilterType::MITCHELL;
			SamplerType timeSamplerType = SamplerType::JITTERED;
			SamplerType cameraSamplerType = SamplerType::CMJ;
			bool visualizeDepth = false;
			double visualizeDepthMaxDistance = 25.0;
		} raytracer;

		struct ToneMapper
		{
			ToneMapperType type = ToneMapperType::LINEAR;
			bool applyGamma = true;
			bool shouldClamp = true;
			double gamma = 2.2;
			double exposure = 0.0;
			double key = 0.18;
			double maxLuminance = 1.0;
		} toneMapper;

		struct SimpleFog
		{
			bool enabled = false;
			Color color = Color::WHITE;
			double distance = 100.0;
			double steepness = 1.0;
			bool heightDispersion = false;
			double height = 100.0;
			double heightSteepness = 1.0;
		} simpleFog;

		struct RootBVH
		{
			bool enabled = false;
			BVHBuildInfo buildInfo;
			FlatBVH bvh;
		} rootBVH;

		struct BoundingBoxes
		{
			bool enabled = false;
			Material material;
		} boundingBoxes;

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
		Material defaultMaterial;

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
			std::vector<Box> boundingBoxes;
			std::vector<Triangle> triangles;
			std::vector<Cylinder> cylinders;
			std::vector<Torus> toruses;
			std::vector<Instance> instances;
			std::vector<CSG> csgs;
			std::vector<BlinnBlob> blinnBlobs;
			std::vector<PrimitiveGroup> primitiveGroups;
			std::vector<Primitive*> visible;
			std::vector<Primitive*> invisible;
		} primitives;

		std::vector<ModelLoaderInfo> models;

		std::map<int, Primitive*> primitivesMap;
		std::map<int, Material*> materialsMap;
		std::map<int, Texture*> texturesMap;
	};
}
