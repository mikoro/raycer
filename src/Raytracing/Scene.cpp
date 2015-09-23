// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <map>
#include <sstream>
#include <stdexcept>

#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/archives/xml.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"
#include "cereal/archives/binary.hpp"

#include "tinyformat/tinyformat.h"

#include "Raytracing/Scene.h"
#include "Raytracing/Primitives/Primitive.h"
#include "Raytracing/Textures/Texture.h"
#include "Raytracing/AABB.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/StringUtils.h"
#include "Utils/Serialization.h"

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
		case 17: return createTestScene17(); break;
		case 18: return createTestScene18(); break;
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

void Scene::addObjScene(const ObjReaderResult& result)
{
	for (const Mesh& mesh : result.meshes)
		primitives.meshes.push_back(mesh);

	for (const Material& material : result.materials)
		materials.push_back(material);

	for (const ColorTexture& colorTexture : result.colorTextures)
		textures.colorTextures.push_back(colorTexture);

	for (const ImageTexture& imageTexture : result.imageTextures)
		textures.imageTextures.push_back(imageTexture);

	primitives.primitiveGroups.push_back(result.primitiveGroup);
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
		ObjReaderResult result = ObjReader::getMeshes(objScene.filePath, objScene.scale, objScene.rotate, objScene.translate, false, 1000000);
		addObjScene(result);
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

	auto sortPrimitive = [&](Primitive* primitive)
	{
		if (primitive->invisible)
			primitives.invisible.push_back(primitive);
		else
			primitives.visible.push_back(primitive);
	};

	for (Plane& plane : primitives.planes)
		sortPrimitive(&plane);

	for (Sphere& sphere : primitives.spheres)
		sortPrimitive(&sphere);

	for (Box& box : primitives.boxes)
		sortPrimitive(&box);

	for (Triangle& triangle : primitives.triangles)
		sortPrimitive(&triangle);

	for (Mesh& mesh : primitives.meshes)
		sortPrimitive(&mesh);

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

	auto mapPrimitive = [&](Primitive* primitive)
	{
		if (primitive->id != 0)
		{
			if (primitivesMap.count(primitive->id))
				throw std::runtime_error(tfm::format("A duplicate primitive id (%d) was found", primitive->id));

			primitivesMap[primitive->id] = primitive;
		}
	};

	for (Primitive* primitive : primitives.invisible)
		mapPrimitive(primitive);

	for (Primitive* primitive : primitives.visible)
		mapPrimitive(primitive);

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

	// POINTER SETTING

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
		for (int primitiveId : primitiveGroup.primitiveIds)
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
		{
			primitive->material = materialsMap[primitive->materialId];

			if (texturesMap.count(primitive->material->colorTextureId))
				primitive->material->colorTexture = texturesMap[primitive->material->colorTextureId];
			else
				throw std::runtime_error(tfm::format("A material (%d) has a non-existent color texture id (%d)", primitive->materialId, primitive->material->colorTextureId));

			// normal and height maps can be null
			if (texturesMap.count(primitive->material->normalMapTextureId))
				primitive->material->normalMapTexture = texturesMap[primitive->material->normalMapTextureId];

			if (texturesMap.count(primitive->material->heightMapTextureId))
				primitive->material->heightMapTexture = texturesMap[primitive->material->heightMapTextureId];
		}
		else
			throw std::runtime_error(tfm::format("A primitive has a non-existent material id (%d)", primitive->materialId));
	};

	for (Primitive* primitive : primitives.invisible)
		setPrimitivePointers(primitive);

	for (Primitive* primitive : primitives.visible)
		setPrimitivePointers(primitive);

	// INITIALIZATION

	for (Plane& plane : primitives.planes)
		plane.initialize();

	for (Sphere& sphere : primitives.spheres)
		sphere.initialize();

	for (Box& box : primitives.boxes)
		box.initialize();

	for (Triangle& triangle : primitives.triangles)
		triangle.initialize();

	for (Mesh& mesh : primitives.meshes)
		mesh.initialize();

	for (Cylinder& cylinder : primitives.cylinders)
		cylinder.initialize();

	for (Torus& torus : primitives.toruses)
		torus.initialize();

	for (BlinnBlob& blinnBlob : primitives.blinnBlobs)
		blinnBlob.initialize();

	for (CSG& csg : primitives.csgs)
		csg.initialize();

	for (PrimitiveGroup& primitiveGroup : primitives.primitiveGroups)
		primitiveGroup.initialize();

	for (Instance& instance : primitives.instances)
		instance.initialize();

	// CAMERA

	camera.initialize();

	// BOUNDING BOXES

	if (boundingBoxes.enabled)
	{
		boundingBoxes.material.colorTexture = &boundingBoxes.texture;

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

	if (rootBVH.enabled)
	{
		rootBVH.bvh.build(primitives.visible, rootBVH.buildInfo);
		primitives.visible.clear();
		primitives.visible.push_back(&rootBVH.bvh);
	}
}
