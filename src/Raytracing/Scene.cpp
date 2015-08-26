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
	boundingBoxes.texture.color = Color(0.8, 0.8, 1.0);
	boundingBoxes.texture.intensity = 1.0;
	boundingBoxes.material.ambientReflectance = Color(0.0, 0.0, 0.0);
	boundingBoxes.material.diffuseReflectance = Color(0.1, 0.1, 0.1);
	boundingBoxes.material.rayTransmittance = 1.0;
	boundingBoxes.material.nonShadowing = true;
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
		case 16: return createTestScene16(); break;
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
	std::map<int, Primitive*> primitivesMap;
	std::map<int, Material*> materialsMap;
	std::map<int, Texture*> texturesMap;

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

	objScenes.clear();

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

	// POINTER MAP GENERATION AND VALIDATION

	for (Primitive* primitive : primitives.all)
	{
		if (primitive->id != 0)
		{
			if (primitivesMap.count(primitive->id))
				throw std::runtime_error(tfm::format("A duplicate primitive id (%d) was found", primitive->id));

			primitivesMap[primitive->id] = primitive;
		}
			
	}

	for (Material& material : materials)
	{
		if (materialsMap.count(material.id))
			throw std::runtime_error(tfm::format("A duplicate material id (%d) was found", material.id));

		materialsMap[material.id] = &material;
	}

	for (Texture* texture : texturesList)
	{
		if (texturesMap.count(texture->id))
			throw std::runtime_error(tfm::format("A duplicate texture id (%d) was found", texture->id));

		texture->initialize();
		texturesMap[texture->id] = texture;
	}

	// INITIALIZATION AND VALIDATION

	for (Instance& instance : primitives.instances)
	{
		if (primitivesMap.count(instance.primitiveId))
			instance.primitive = primitivesMap[instance.primitiveId];
		else
			throw std::runtime_error(tfm::format("An instance has a non-existent primitive id (%d)", instance.primitiveId));
	}

	for (Primitive* primitive : primitives.all)
	{
		if (materialsMap.count(primitive->materialId))
		{
			primitive->material = materialsMap[primitive->materialId];

			if (texturesMap.count(primitive->material->colorTextureId))
				primitive->material->colorTexture = texturesMap[primitive->material->colorTextureId];
			else
				throw std::runtime_error(tfm::format("A material (%d) has a non-existent texture id (%d)", primitive->materialId, primitive->material->colorTextureId));

			if (texturesMap.count(primitive->material->normalMapTextureId))
				primitive->material->normalMapTexture = texturesMap[primitive->material->normalMapTextureId];
		}
		else
			throw std::runtime_error(tfm::format("A primitive has a non-existent material id (%d)", primitive->materialId));

		primitive->initialize();
	}

	// CAMERA

	camera.initialize();

	// BOUNDING BOXES

	if (boundingBoxes.enabled)
	{
		boundingBoxes.material.colorTexture = &boundingBoxes.texture;

		for (Primitive* primitive : primitives.all)
		{
			AABB aabb = primitive->getAABB();

			Box bbox;
			bbox.material = &boundingBoxes.material;
			bbox.position = aabb.getCenter();
			bbox.extent = aabb.getExtent();

			primitives.boundingBoxes.push_back(bbox);
		}

		for (Box& box : primitives.boundingBoxes)
			primitives.all.push_back(&box);
	}

	// ROOT BVH

	if (rootBVH.enabled)
	{
		rootBVH.bvh.build(primitives.all, rootBVH.buildInfo);
		primitives.all.clear();
		primitives.all.push_back(&rootBVH.bvh);
	}
}
