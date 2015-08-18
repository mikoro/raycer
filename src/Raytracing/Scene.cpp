// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <map>
#include <sstream>
#include <stdexcept>

#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/archives/xml.hpp"
#include "cereal/types/vector.hpp"
//#include "cereal/archives/binary.hpp"

#include "tinyformat/tinyformat.h"

#include "Raytracing/Scene.h"
#include "Raytracing/Primitives/Primitive.h"
#include "Raytracing/Textures/Texture.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/StringUtils.h"
#include "Utils/Serialization.h"
#include "Utils/ObjReader.h"

using namespace Raycer;

Scene::Scene()
{
}

Scene Scene::createTestScene(int number)
{
	App::getLog().logInfo("Creating test scene number %d", number);

	switch (number)
	{
		case 1: return createTestScene1(); break;
		case 2: return createTestScene2(); break;
		case 3: return createTestScene3(); break;
		case 4: return createTestScene4(); break;
		case 5: return createTestScene5(); break;
		case 6: return createTestScene6(); break;
		case 7: return createTestScene7(); break;
		case 8: return createTestScene8(); break;
		case 9: return createTestScene9(); break;
		case 10: return createTestScene10(); break;
		case 11: return createTestScene11(); break;
		case 12: return createTestScene12(); break;
		case 13: return createTestScene13(); break;
		case 14: return createTestScene14(); break;
		case 15: return createTestScene15(); break;
		default: throw std::runtime_error("Unknown test scene number");
	}
}

Scene Scene::loadFromFile(const std::string& fileName)
{
	App::getLog().logInfo("Loading scene from %s", fileName);

	std::ifstream file(fileName, std::ios::binary);

	if (!file.good())
		throw std::runtime_error("Could not open the scene file");

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
		// TODO: will not compile
		//cereal::BinaryInputArchive archive(file);
		//archive(scene);
	}
	else
		throw std::runtime_error("Unsupported scene file format");

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
		throw std::runtime_error("Could not open the scene file");

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
		// TODO: will not compile
		//cereal::BinaryOutputArchive archive(file);
		//archive(cereal::make_nvp("scene", *this));
	}
	else
		throw std::runtime_error("Unsupported scene file format");
}

std::string Scene::saveToJsonString() const
{
	App::getLog().logInfo("Saving the scene to JSON string");

	std::stringstream ss;
	cereal::JSONOutputArchive archive(ss);
	archive(*this);

	return ss.str();
}

std::string Scene::saveToXmlString() const
{
	App::getLog().logInfo("Saving the scene to XML string");

	std::stringstream ss;
	cereal::XMLOutputArchive archive(ss);
	archive(*this);

	return ss.str();
}

void Scene::initialize()
{
	std::vector<Texture*> texturesList;
	std::map<int, Texture*> texturesMap;
	std::map<int, Material*> materialsMap;

	// OBJ SCENES
	for (const ObjScene& objScene : objScenes)
	{
		ObjReaderResult result = ObjReader::getMeshes(objScene.filePath, objScene.scale, 1000000);

		for (Mesh& mesh : result.meshes)
			primitives.meshes.push_back(mesh);

		for (Material& material : result.materials)
			materials.push_back(material);

		for (ColorTexture& colorTexture : result.colorTextures)
			textures.colorTextures.push_back(colorTexture);

		for (ImageTexture& imageTexture : result.imageTextures)
			textures.imageTextures.push_back(imageTexture);
	}

	// TEXTURE POINTERS

	for (ColorTexture& texture : textures.colorTextures)
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

	for (Plane& plane : primitives.planes)
		primitives.all.push_back(&plane);

	for (Sphere& sphere : primitives.spheres)
		primitives.all.push_back(&sphere);

	for (Box& box : primitives.boxes)
		primitives.all.push_back(&box);

	for (Triangle& triangle : primitives.triangles)
		primitives.all.push_back(&triangle);

	for (Mesh& mesh : primitives.meshes)
		primitives.all.push_back(&mesh);

	for (Instance& instance : primitives.instances)
		primitives.all.push_back(&instance);

	// INITIALIZATION AND VALIDATION

	for (Texture* texture : texturesList)
	{
		texture->initialize();
		texturesMap[texture->id] = texture;
	}

	for (Material& material : materials)
		materialsMap[material.id] = &material;

	for (Primitive* primitive : primitives.all)
	{
		if (materialsMap.count(primitive->materialId))
		{
			primitive->material = materialsMap[primitive->materialId];

			if (texturesMap.count(primitive->material->colorTextureId))
				primitive->material->colorTexture = texturesMap[primitive->material->colorTextureId];
			else
				throw std::runtime_error(tfm::format("A material (%d) has an invalid texture id (%d)", primitive->materialId, primitive->material->colorTextureId));

			if (texturesMap.count(primitive->material->normalMapTextureId))
				primitive->material->normalMapTexture = texturesMap[primitive->material->normalMapTextureId];
		}
		else
			throw std::runtime_error(tfm::format("A primitive has an invalid material id (%d)", primitive->materialId));

		primitive->initialize();
	}

	// CAMERA

	camera.initialize();

	// BOUNDING BOXES

	if (boundingBoxes.enabled)
	{
		boundingBoxes.texture.color = Color(0.8, 0.8, 1.0);
		boundingBoxes.texture.intensity = 1.0;
		boundingBoxes.material.colorTexture = &boundingBoxes.texture;
		boundingBoxes.material.diffuseReflectance = Color(0.0, 0.0, 0.0);
		boundingBoxes.material.diffuseReflectance = Color(0.1, 0.1, 0.1);
		boundingBoxes.material.rayTransmittance = 1.0;
		boundingBoxes.material.nonShadowing = true;

		for (Primitive* primitive : primitives.all)
		{
			AABB aabb = primitive->getAABB();

			Box bbox;
			bbox.material = &boundingBoxes.material;
			bbox.position = aabb.center;
			bbox.extent = aabb.extent;

			primitives.boundingBoxes.push_back(bbox);
		}

		for (Box& box : primitives.boundingBoxes)
			primitives.all.push_back(&box);
	}

	// ROOT BVH

	if (rootBVH.enabled)
	{
		primitives.rootBVH.build(primitives.all, rootBVH.buildInfo);
		primitives.all.clear();
		primitives.all.push_back(&primitives.rootBVH);
	}
}
