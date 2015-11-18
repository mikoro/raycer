// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

// when using precompiled headers with this file, the deserialization of XML files will crash in release mode
//#include "stdafx.h"

#include "tinyformat/tinyformat.h"

#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/archives/xml.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"

#include "Raytracing/Scene.h"
#include "Raytracing/Primitives/Primitive.h"
#include "Raytracing/Textures/Texture.h"
#include "Raytracing/AABB.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/StringUtils.h"

using namespace Raycer;

Scene::Scene()
{
	if (boundingBoxes.useDefaultMaterial)
	{
		boundingBoxes.material = Material();
		boundingBoxes.material.ambientReflectance = Color(0.1, 0.1, 0.2);
		boundingBoxes.material.diffuseReflectance = Color(0.1, 0.1, 0.2);
		boundingBoxes.material.rayTransmittance = 1.0;
		boundingBoxes.material.nonShadowing = true;
	}
}

Scene Scene::createTestScene(uint64_t number)
{
	App::getLog().logInfo("Creating test scene number %d", number);

	switch (number)
	{
		case 1: return createTestScene1();
		case 2: return createTestScene2();
		case 3: return createTestScene3();
		case 4: return createTestScene4();
		case 5: return createTestScene5();
		case 6: return createTestScene6();
		case 7: return createTestScene7();
		case 8: return createTestScene8();
		case 9: return createTestScene9();
		case 10: return createTestScene10();
		case 11: return createTestScene11();
		case 12: return createTestScene12();
		case 13: return createTestScene13();
		case 14: return createTestScene14();
		case 15: return createTestScene15();
		case 16: return createTestScene16();
		case 17: return createTestScene17();
		case 18: return createTestScene18();
		case 19: return createTestScene19();
		default: throw std::runtime_error("Unknown test scene number");
	}
}

Scene Scene::loadFromFile(const std::string& fileName)
{
	App::getLog().logInfo("Loading scene from %s", fileName);

	std::ifstream file(fileName, std::ios::binary);

	if (!file.good())
		throw std::runtime_error("Could not open the scene file for loading");

	Scene scene;

	if (StringUtils::endsWith(fileName, ".json"))
	{
		cereal::JSONInputArchive archive(file);
		archive(scene);
	}
	else if (StringUtils::endsWith(fileName, ".xml"))
	{
		cereal::XMLInputArchive archive(file);
		archive(scene);
	}
	else if (StringUtils::endsWith(fileName, ".bin"))
	{
		cereal::BinaryInputArchive archive(file);
		archive(scene);
	}
	else
		throw std::runtime_error("Unsupported scene file format");

	file.close();

	return scene;
}

Scene Scene::loadFromJsonString(const std::string& text)
{
	App::getLog().logInfo("Loading scene from JSON string");

	Scene scene;
	std::stringstream ss(text);
	cereal::JSONInputArchive archive(ss);
	archive(scene);

	return scene;
}

Scene Scene::loadFromXmlString(const std::string& text)
{
	App::getLog().logInfo("Loading scene from XML string");

	Scene scene;
	std::stringstream ss(text);
	cereal::XMLInputArchive archive(ss);
	archive(scene);

	return scene;
}

void Scene::saveToFile(const std::string& fileName) const
{
	App::getLog().logInfo("Saving scene to %s", fileName);

	std::ofstream file(fileName, std::ios::binary);

	if (!file.good())
		throw std::runtime_error("Could not open the scene file for saving");

	if (StringUtils::endsWith(fileName, ".json"))
	{
		cereal::JSONOutputArchive archive(file);
		archive(cereal::make_nvp("scene", *this));
	}
	else if (StringUtils::endsWith(fileName, ".xml"))
	{
		cereal::XMLOutputArchive archive(file);
		archive(cereal::make_nvp("scene", *this));
	}
	else if (StringUtils::endsWith(fileName, ".bin"))
	{
		cereal::BinaryOutputArchive archive(file);
		archive(cereal::make_nvp("scene", *this));
	}
	else
		throw std::runtime_error("Unsupported scene file format");

	file.close();
}

std::string Scene::getJsonString() const
{
	App::getLog().logInfo("Saving the scene to JSON string");

	std::stringstream ss;

	// force desctructor invocation for flushing
	{
		cereal::JSONOutputArchive archive(ss);
		archive(cereal::make_nvp("scene", *this));
	}

	return ss.str();
}

std::string Scene::getXmlString() const
{
	App::getLog().logInfo("Saving the scene to XML string");

	std::stringstream ss;

	// force desctructor invocation for flushing
	{
		cereal::XMLOutputArchive archive(ss);
		archive(cereal::make_nvp("scene", *this));
	}

	return ss.str();
}

void Scene::addModel(const ModelLoaderResult& result)
{
	for (const Triangle& triangle : result.triangles)
		primitives.triangles.push_back(triangle);

	for (const Material& material : result.materials)
		materials.push_back(material);

	for (const ImageTexture& imageTexture : result.textures)
		textures.imageTextures.push_back(imageTexture);

	for (const PrimitiveGroup& group : result.groups)
		primitives.primitiveGroups.push_back(group);

	for (const Instance& instance : result.instances)
		primitives.instances.push_back(instance);
}

void Scene::initialize()
{
	Log& log = App::getLog();
	log.logInfo("Initializing scene");

	auto startTime = std::chrono::high_resolution_clock::now();

	std::vector<Texture*> texturesList;

	// MODELS

	for (const ModelLoaderInfo& modelInfo : models)
	{
		if (StringUtils::endsWith(modelInfo.modelFilePath, ".obj"))
			addModel(ModelLoader::readObjFile(modelInfo));
		else if (StringUtils::endsWith(modelInfo.modelFilePath, ".ply"))
			addModel(ModelLoader::readPlyFile(modelInfo));
	}

	models.clear();

	// TEXTURE POINTERS

	for (ColorTexture& texture : textures.colorTextures)
		texturesList.push_back(&texture);

	for (ColorGradientTexture& texture : textures.colorGradientTextures)
		texturesList.push_back(&texture);

	for (CheckerTexture& texture : textures.checkerTextures)
		texturesList.push_back(&texture);

	for (ImageTexture& texture : textures.imageTextures)
		texturesList.push_back(&texture);

	for (PerlinNoiseTexture& texture : textures.perlinNoiseTextures)
		texturesList.push_back(&texture);

	for (ValueNoiseTexture& texture : textures.valueNoiseTextures)
		texturesList.push_back(&texture);

	for (CellNoiseTexture& texture : textures.cellNoiseTextures)
		texturesList.push_back(&texture);

	for (MarbleTexture& texture : textures.marbleTextures)
		texturesList.push_back(&texture);

	for (WoodTexture& texture : textures.woodTextures)
		texturesList.push_back(&texture);

	for (FireTexture& texture : textures.fireTextures)
		texturesList.push_back(&texture);

	for (AtmosphereTexture& texture : textures.atmosphereTextures)
		texturesList.push_back(&texture);

	for (VoronoiTexture& texture : textures.voronoiTextures)
		texturesList.push_back(&texture);

	// PRIMITIVE POINTERS

	auto sortPrimitive = [&](Primitive* primitive)
	{
		if (primitive->invisible)
			primitives.invisible.push_back(primitive);
		else
			primitives.visible.push_back(primitive);
	};

	for (Triangle& triangle : primitives.triangles)
		sortPrimitive(&triangle);

	for (Plane& plane : primitives.planes)
		sortPrimitive(&plane);

	for (Sphere& sphere : primitives.spheres)
		sortPrimitive(&sphere);

	for (Box& box : primitives.boxes)
		sortPrimitive(&box);

	for (Cylinder& cylinder : primitives.cylinders)
		sortPrimitive(&cylinder);

	for (Torus& torus : primitives.toruses)
		sortPrimitive(&torus);

	for (BlinnBlob& blinnBlob : primitives.blinnBlobs)
		sortPrimitive(&blinnBlob);

	for (CSG& csg : primitives.csgs)
		sortPrimitive(&csg);

	for (PrimitiveGroup& primitiveGroup : primitives.primitiveGroups)
		sortPrimitive(&primitiveGroup);

	for (Instance& instance : primitives.instances)
		sortPrimitive(&instance);

	// POINTER MAP GENERATION

	for (Material& material : materials)
	{
		if (materialsMap.count(material.id))
			throw std::runtime_error(tfm::format("A duplicate material id was found (id: %s)", material.id));

		materialsMap[material.id] = &material;
	}

	// add default material
	if (!materialsMap.count(0))
		materialsMap[0] = &defaultMaterial;

	for (Texture* texture : texturesList)
	{
		if (texture->id == 0)
			throw std::runtime_error(tfm::format("A texture must have a non-zero id (id: %s, type: %s)", texture->id, typeid(*texture).name()));

		if (texturesMap.count(texture->id))
			throw std::runtime_error(tfm::format("A duplicate texture id was found (id: %s, type: %s)", texture->id, typeid(*texture).name()));

		texturesMap[texture->id] = texture;
	}

	auto mapPrimitive = [&](Primitive* primitive)
	{
		if (primitive->id == 0)
			throw std::runtime_error(tfm::format("A primitive must have a non-zero id (id: %s, type: %s)", primitive->id, typeid(*primitive).name()));

		if (primitivesMap.count(primitive->id))
			throw std::runtime_error(tfm::format("A duplicate primitive id was found (id: %s, type: %s)", primitive->id, typeid(*primitive).name()));

		primitivesMap[primitive->id] = primitive;
	};

	for (Primitive* primitive : primitives.invisible)
		mapPrimitive(primitive);

	for (Primitive* primitive : primitives.visible)
		mapPrimitive(primitive);

	// POINTER SETTING

	for (Material& material : materials)
	{
		if (texturesMap.count(material.ambientMapTextureId))
			material.ambientMapTexture = texturesMap[material.ambientMapTextureId];

		if (texturesMap.count(material.diffuseMapTextureId))
			material.diffuseMapTexture = texturesMap[material.diffuseMapTextureId];

		if (texturesMap.count(material.specularMapTextureId))
			material.specularMapTexture = texturesMap[material.specularMapTextureId];

		if (texturesMap.count(material.emittanceMapTextureId))
			material.emittanceMapTexture = texturesMap[material.emittanceMapTextureId];

		if (texturesMap.count(material.rayReflectanceMapTextureId))
			material.rayReflectanceMapTexture = texturesMap[material.rayReflectanceMapTextureId];

		if (texturesMap.count(material.rayTransmittanceMapTextureId))
			material.rayTransmittanceMapTexture = texturesMap[material.rayTransmittanceMapTextureId];

		if (texturesMap.count(material.normalMapTextureId))
			material.normalMapTexture = texturesMap[material.normalMapTextureId];

		if (texturesMap.count(material.maskMapTextureId))
			material.maskMapTexture = texturesMap[material.maskMapTextureId];

		if (texturesMap.count(material.heightMapTextureId))
			material.heightMapTexture = texturesMap[material.heightMapTextureId];
	}

	for (CSG& csg : primitives.csgs)
	{
		if (primitivesMap.count(csg.leftPrimitiveId))
			csg.leftPrimitive = primitivesMap[csg.leftPrimitiveId];
		else
			throw std::runtime_error(tfm::format("A CSG has a non-existent left primitive id (%d)", csg.leftPrimitiveId));

		if (primitivesMap.count(csg.rightPrimitiveId))
			csg.rightPrimitive = primitivesMap[csg.rightPrimitiveId];
		else
			throw std::runtime_error(tfm::format("A CSG has a non-existent right primitive id (%d)", csg.rightPrimitiveId));
	}

	for (Instance& instance : primitives.instances)
	{
		if (primitivesMap.count(instance.primitiveId))
			instance.primitive = primitivesMap[instance.primitiveId];
		else
			throw std::runtime_error(tfm::format("An instance has a non-existent primitive id (%d)", instance.primitiveId));
	}

	for (PrimitiveGroup& primitiveGroup : primitives.primitiveGroups)
	{
		for (uint64_t primitiveId : primitiveGroup.primitiveIds)
		{
			if (primitivesMap.count(primitiveId))
				primitiveGroup.primitives.push_back(primitivesMap[primitiveId]);
			else
				throw std::runtime_error(tfm::format("A primitive group has a non-existent primitive id (%d)", primitiveId));
		}
	}

	auto setPrimitivePointers = [&](Primitive* primitive)
	{
		if (materialsMap.count(primitive->materialId))
			primitive->material = materialsMap[primitive->materialId];
		else
			throw std::runtime_error(tfm::format("A primitive has a non-existent material id (%d)", primitive->materialId));
	};

	for (Primitive* primitive : primitives.invisible)
		setPrimitivePointers(primitive);

	for (Primitive* primitive : primitives.visible)
		setPrimitivePointers(primitive);

	// INITIALIZATION

	for (Texture* texture : texturesList)
		texture->initialize();

	for (Triangle& triangle : primitives.triangles)
		triangle.initialize(*this);

	for (Plane& plane : primitives.planes)
		plane.initialize(*this);

	for (Sphere& sphere : primitives.spheres)
		sphere.initialize(*this);

	for (Box& box : primitives.boxes)
		box.initialize(*this);

	for (Cylinder& cylinder : primitives.cylinders)
		cylinder.initialize(*this);

	for (Torus& torus : primitives.toruses)
		torus.initialize(*this);

	for (BlinnBlob& blinnBlob : primitives.blinnBlobs)
		blinnBlob.initialize(*this);

	for (CSG& csg : primitives.csgs)
		csg.initialize(*this);

	for (PrimitiveGroup& primitiveGroup : primitives.primitiveGroups)
		primitiveGroup.initialize(*this);

	for (Instance& instance : primitives.instances)
		instance.initialize(*this);

	// CAMERA

	camera.initialize();

	// BOUNDING BOXES

	if (boundingBoxes.enabled)
	{
		for (Primitive* primitive : primitives.visible)
		{
			AABB aabb = primitive->getAABB();

			Box bbox;
			bbox.material = &boundingBoxes.material;
			bbox.position = aabb.getCenter();
			bbox.extent = aabb.getExtent();

			primitives.boundingBoxes.push_back(bbox);
		}

		for (Box& box : primitives.boundingBoxes)
			primitives.visible.push_back(&box);
	}

	// ROOT BVH

	primitives.visibleOriginal = primitives.visible;

	if (rootBVH.enabled)
	{
		if (rootBVH.bvh.hasBeenBuilt)
			rootBVH.bvh.restore(*this);
		else
			rootBVH.bvh.build(primitives.visible, rootBVH.buildInfo);

		primitives.visible.clear();
		primitives.visible.push_back(&rootBVH.bvh);
	}

	auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();

	log.logInfo("Scene initialization finished (time: %d ms)", milliseconds);
}

void Scene::rebuildRootBVH()
{
	if (!rootBVH.enabled)
		return;

	rootBVH.bvh.build(primitives.visibleOriginal, rootBVH.buildInfo);

	primitives.visible.clear();
	primitives.visible.push_back(&rootBVH.bvh);
}
