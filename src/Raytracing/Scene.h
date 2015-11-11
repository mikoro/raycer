// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <map>
#include <vector>

#include "cereal/cereal.hpp"

#include "Raytracing/Camera.h"
#include "Raytracing/Textures/ColorTexture.h"
#include "Raytracing/Textures/ColorGradientTexture.h"
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
#include "Raytracing/Tracers/Tracer.h"
#include "Rendering/Samplers/Sampler.h"
#include "Rendering/Filters/Filter.h"
#include "Rendering/ToneMappers/ToneMapper.h"
#include "Math/Color.h"
#include "Utils/ModelLoader.h"

namespace Raycer
{
	class Primitive;

	class Scene
	{
	public:

		Scene();

		static Scene createTestScene(uint64_t number);
		static Scene loadFromFile(const std::string& fileName);
		static Scene loadFromJsonString(const std::string& text);
		static Scene loadFromXmlString(const std::string& text);

		void saveToFile(const std::string& fileName) const;
		std::string getJsonString() const;
		std::string getXmlString() const;

		void addModel(const ModelLoaderResult& result);
		void initialize();
		void rebuildRootBVH();

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
		static Scene createTestScene19();

		static const uint64_t TEST_SCENE_COUNT = 19;

		struct General
		{
			TracerType tracerType = TracerType::RAY;
			SamplerType multiSamplerType = SamplerType::CENTER;
			FilterType multiSamplerFilterType = FilterType::MITCHELL;
			SamplerType timeSamplerType = SamplerType::JITTERED;
			SamplerType cameraSamplerType = SamplerType::CMJ;
			uint64_t maxRayIterations = 3;
			uint64_t maxPathLength = 3;
			uint64_t pathSamples = 1;
			uint64_t multiSamplesSqrt = 1;
			uint64_t timeSamples = 1;
			uint64_t cameraSamplesSqrt = 0;
			double rayStartOffset = 0.00001;
			Color backgroundColor = Color(0.0, 0.0, 0.0);
			Color offLensColor = Color(0.0, 0.0, 0.0);
			bool visualizeDepth = false;
			double visualizeDepthMaxDistance = 25.0;
			bool enableNormalMapping = true;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(tracerType),
					CEREAL_NVP(multiSamplerType),
					CEREAL_NVP(multiSamplerFilterType),
					CEREAL_NVP(timeSamplerType),
					CEREAL_NVP(cameraSamplerType),
					CEREAL_NVP(maxRayIterations),
					CEREAL_NVP(maxPathLength),
					CEREAL_NVP(pathSamples),
					CEREAL_NVP(multiSamplesSqrt),
					CEREAL_NVP(timeSamples),
					CEREAL_NVP(cameraSamplesSqrt),
					CEREAL_NVP(rayStartOffset),
					CEREAL_NVP(backgroundColor),
					CEREAL_NVP(offLensColor),
					CEREAL_NVP(visualizeDepth),
					CEREAL_NVP(visualizeDepthMaxDistance),
					CEREAL_NVP(enableNormalMapping));
			}

		} general;

		Camera camera;

		struct ToneMapper
		{
			ToneMapperType type = ToneMapperType::LINEAR;
			bool applyGamma = true;
			bool shouldClamp = true;
			double gamma = 2.2;
			double exposure = 0.0;
			double key = 0.18;
			bool enableAveraging = false;
			double averageAlpha = 0.1;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(type),
					CEREAL_NVP(applyGamma),
					CEREAL_NVP(shouldClamp),
					CEREAL_NVP(gamma),
					CEREAL_NVP(exposure),
					CEREAL_NVP(key),
					CEREAL_NVP(enableAveraging),
					CEREAL_NVP(averageAlpha));
			}

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

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(enabled),
					CEREAL_NVP(color),
					CEREAL_NVP(distance),
					CEREAL_NVP(steepness),
					CEREAL_NVP(heightDispersion),
					CEREAL_NVP(height),
					CEREAL_NVP(heightSteepness));
			}

		} simpleFog;

		struct RootBVH
		{
			bool enabled = false;
			BVHBuildInfo buildInfo;
			FlatBVH bvh;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(enabled),
					CEREAL_NVP(buildInfo),
					CEREAL_NVP(bvh));
			}

		} rootBVH;

		struct BoundingBoxes
		{
			bool enabled = false;
			bool useDefaultMaterial = true;
			Material material;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(enabled),
					CEREAL_NVP(useDefaultMaterial),
					CEREAL_NVP(material));
			}

		} boundingBoxes;

		struct Textures
		{
			std::vector<ColorTexture> colorTextures;
			std::vector<ColorGradientTexture> colorGradientTextures;
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

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(colorTextures),
					CEREAL_NVP(colorGradientTextures),
					CEREAL_NVP(checkerTextures),
					CEREAL_NVP(imageTextures),
					CEREAL_NVP(perlinNoiseTextures),
					CEREAL_NVP(valueNoiseTextures),
					CEREAL_NVP(cellNoiseTextures),
					CEREAL_NVP(marbleTextures),
					CEREAL_NVP(woodTextures),
					CEREAL_NVP(fireTextures),
					CEREAL_NVP(atmosphereTextures),
					CEREAL_NVP(voronoiTextures));
			}

		} textures;

		std::vector<Material> materials;
		Material defaultMaterial;

		struct Lights
		{
			AmbientLight ambientLight;
			std::vector<DirectionalLight> directionalLights;
			std::vector<PointLight> pointLights;
			std::vector<SpotLight> spotLights;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(ambientLight),
					CEREAL_NVP(directionalLights),
					CEREAL_NVP(pointLights),
					CEREAL_NVP(spotLights));
			}

		} lights;

		std::vector<ModelLoaderInfo> models;

		struct Primitives
		{
			std::vector<Triangle> triangles;
			std::vector<Plane> planes;
			std::vector<Sphere> spheres;
			std::vector<Box> boxes;
			std::vector<Cylinder> cylinders;
			std::vector<Torus> toruses;
			std::vector<BlinnBlob> blinnBlobs;
			std::vector<CSG> csgs;
			std::vector<PrimitiveGroup> primitiveGroups;
			std::vector<Instance> instances;
			std::vector<Box> boundingBoxes;
			std::vector<Primitive*> visible;
			std::vector<Primitive*> invisible;
			std::vector<Primitive*> visibleOriginal;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(triangles),
					CEREAL_NVP(planes),
					CEREAL_NVP(spheres),
					CEREAL_NVP(boxes),
					CEREAL_NVP(cylinders),
					CEREAL_NVP(toruses),
					CEREAL_NVP(blinnBlobs),
					CEREAL_NVP(csgs),
					CEREAL_NVP(primitiveGroups),
					CEREAL_NVP(instances));
			}

		} primitives;

		std::map<uint64_t, Primitive*> primitivesMap;
		std::map<uint64_t, Material*> materialsMap;
		std::map<uint64_t, Texture*> texturesMap;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(general),
				CEREAL_NVP(camera),
				CEREAL_NVP(toneMapper),
				CEREAL_NVP(simpleFog),
				CEREAL_NVP(rootBVH),
				CEREAL_NVP(boundingBoxes),
				CEREAL_NVP(textures),
				CEREAL_NVP(materials),
				CEREAL_NVP(lights),
				CEREAL_NVP(models),
				CEREAL_NVP(primitives));
		}
	};
}
