// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include "tinyformat/tinyformat.h"

#include "Utils/ObjModelLoader.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/StringUtils.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Math/Matrix4x4.h"

using namespace Raycer;
using namespace boost::filesystem;

ModelLoaderResult ObjModelLoader::readFile(const ModelLoaderInfo& info)
{
	Log& log = App::getLog();

	log.logInfo("Reading OBJ file (%s)", info.modelFilePath);

	auto startTime = std::chrono::high_resolution_clock::now();

	std::ifstream file(info.modelFilePath);

	if (!file.good())
		throw std::runtime_error("Could not open the OBJ file");

	ModelLoaderResult result;

	PrimitiveGroup combinedGroup;
	combinedGroup.id = info.combinedGroupId;
	combinedGroup.invisible = info.invisibleCombinedGroup;

	Instance combinedGroupInstance;
	combinedGroupInstance.id = info.combinedGroupInstanceId;
	combinedGroupInstance.primitiveId = combinedGroup.id;

	int currentId = info.idStartOffset;
	int currentMaterialId = info.defaultMaterialId;

	std::map<std::string, int> materialsMap;
	std::string objFileDirectory = boost::filesystem::absolute(info.modelFilePath).parent_path().string();

	Matrix4x4 scaling = Matrix4x4::scale(info.scale);
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(info.rotate);
	Matrix4x4 translation = Matrix4x4::translate(info.translate);
	Matrix4x4 transformation = translation * rotation * scaling;
	Matrix4x4 transformationInvT = transformation.inverted().transposed();

	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;

	std::string line;

	while (std::getline(file, line))
	{
		int lineIndex = 0;
		std::string part;
		StringUtils::readUntilSpace(line, lineIndex, part);

		if (part.size() == 0)
			continue;

		// new material file
		if (part == "mtllib" && !info.ignoreMaterials)
		{
			StringUtils::readUntilSpace(line, lineIndex, part);
			processMaterialFile(objFileDirectory, part, info, result, materialsMap, currentId);
			continue;
		}

		// new group
		if (part == "g")
		{
			if (info.enableGroups)
			{
				result.groups.push_back(PrimitiveGroup());
				result.groups.back().id = ++currentId;
				result.groups.back().invisible = info.invisibleGroups;

				if (info.enableGroupsInstances)
				{
					result.instances.push_back(Instance());
					result.instances.back().id = ++currentId;
					result.instances.back().primitiveId = result.groups.back().id;
				}
			}

			continue;
		}

		// select material
		if (part == "usemtl" && !info.ignoreMaterials)
		{
			StringUtils::readUntilSpace(line, lineIndex, part);

			if (materialsMap.count(part))
				currentMaterialId = materialsMap[part];
			else
			{
				log.logWarning("Could not find material named \"%s\"", part);
				currentMaterialId = info.defaultMaterialId;
			}

			continue;
		}

		// vertex
		if (part == "v")
		{
			Vector3 vertex;

			StringUtils::readUntilSpace(line, lineIndex, part);
			vertex.x = StringUtils::parseDouble(part);

			StringUtils::readUntilSpace(line, lineIndex, part);
			vertex.y = StringUtils::parseDouble(part);

			StringUtils::readUntilSpace(line, lineIndex, part);
			vertex.z = StringUtils::parseDouble(part);

			vertices.push_back(transformation.transformPosition(vertex));
			continue;
		}

		// normal
		if (part == "vn")
		{
			Vector3 normal;

			StringUtils::readUntilSpace(line, lineIndex, part);
			normal.x = StringUtils::parseDouble(part);

			StringUtils::readUntilSpace(line, lineIndex, part);
			normal.y = StringUtils::parseDouble(part);

			StringUtils::readUntilSpace(line, lineIndex, part);
			normal.z = StringUtils::parseDouble(part);

			normals.push_back(transformationInvT.transformDirection(normal).normalized());
			continue;
		}

		// texcoord
		if (part == "vt")
		{
			Vector2 texcoord;

			StringUtils::readUntilSpace(line, lineIndex, part);
			texcoord.x = StringUtils::parseDouble(part);

			StringUtils::readUntilSpace(line, lineIndex, part);
			texcoord.y = StringUtils::parseDouble(part);

			texcoords.push_back(texcoord);
			continue;
		}

		// face
		if (part == "f")
			processFace(line.substr(lineIndex), vertices, normals, texcoords, info, result, combinedGroup, currentId, currentMaterialId);
	}

	file.close();

	if (info.enableCombinedGroup)
	{
		result.groups.push_back(combinedGroup);

		if (info.enableCombinedGroupInstance)
			result.instances.push_back(combinedGroupInstance);
	}

	auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
	int milliseconds = (int)std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();

	log.logInfo("OBJ file reading finished (time: %d ms, groups: %s, triangles: %s, materials: %s, textures: %s)", milliseconds, result.groups.size(), result.triangles.size(), result.materials.size(), result.textures.size());

	return result;
}

namespace
{
	std::string getAbsolutePath(const std::string& rootDirectory, const std::string& relativeFilePath)
	{
		path tempPath(rootDirectory);
		tempPath.append(relativeFilePath.begin(), relativeFilePath.end());
		std::string tempPathString = tempPath.string();
		std::replace(tempPathString.begin(), tempPathString.end(), '\\', '/');
		return tempPathString;
	}
}

void ObjModelLoader::processMaterialFile(const std::string& objFileDirectory, const std::string& mtlFilePath, const ModelLoaderInfo& info, ModelLoaderResult& result, std::map<std::string, int>& materialsMap, int& currentId)
{
	std::string absoluteMtlFilePath = getAbsolutePath(objFileDirectory, mtlFilePath);
	App::getLog().logInfo("Reading MTL file (%s)", absoluteMtlFilePath);
	std::ifstream file(absoluteMtlFilePath);

	if (!file.good())
		throw std::runtime_error("Could not open the MTL file");

	Material currentMaterial = info.baseMaterial;
	bool materialPending = false;
	bool hasAmbientMap = false;
	bool hasDiffuseMap = false;
	bool hasSpecularMap = false;
	bool hasNormalMap = false;
	bool hasMaskMap = false;
	bool hasHeightMap = false;

	std::string line;

	while (std::getline(file, line))
	{
		int lineIndex = 0;
		std::string part;
		StringUtils::readUntilSpace(line, lineIndex, part);

		if (part.size() == 0)
			continue;

		// new material
		if (part == "newmtl")
		{
			if (materialPending)
			{
				result.materials.push_back(currentMaterial);

				hasAmbientMap = false;
				hasDiffuseMap = false;
				hasSpecularMap = false;
				hasNormalMap = false;
				hasMaskMap = false;
				hasHeightMap = false;
			}

			materialPending = true;

			currentMaterial = info.baseMaterial;
			currentMaterial.id = ++currentId;

			StringUtils::readUntilSpace(line, lineIndex, part);
			materialsMap[part] = currentMaterial.id;

			continue;
		}

		// ambient reflectance
		if (part == "Ka")
		{
			StringUtils::readUntilSpace(line, lineIndex, part);
			currentMaterial.ambientReflectance.r = StringUtils::parseDouble(part);

			StringUtils::readUntilSpace(line, lineIndex, part);
			currentMaterial.ambientReflectance.g = StringUtils::parseDouble(part);

			StringUtils::readUntilSpace(line, lineIndex, part);
			currentMaterial.ambientReflectance.b = StringUtils::parseDouble(part);

			continue;
		}

		// diffuse reflectance
		if (part == "Kd")
		{
			StringUtils::readUntilSpace(line, lineIndex, part);
			currentMaterial.diffuseReflectance.r = StringUtils::parseDouble(part);

			StringUtils::readUntilSpace(line, lineIndex, part);
			currentMaterial.diffuseReflectance.g = StringUtils::parseDouble(part);

			StringUtils::readUntilSpace(line, lineIndex, part);
			currentMaterial.diffuseReflectance.b = StringUtils::parseDouble(part);

			continue;
		}

		// specular reflectance
		if (part == "Ks")
		{
			StringUtils::readUntilSpace(line, lineIndex, part);
			currentMaterial.specularReflectance.r = StringUtils::parseDouble(part);

			StringUtils::readUntilSpace(line, lineIndex, part);
			currentMaterial.specularReflectance.g = StringUtils::parseDouble(part);

			StringUtils::readUntilSpace(line, lineIndex, part);
			currentMaterial.specularReflectance.b = StringUtils::parseDouble(part);

			continue;
		}

		// shininess
		if (part == "Ns")
		{
			StringUtils::readUntilSpace(line, lineIndex, part);
			currentMaterial.shininess = StringUtils::parseDouble(part);

			continue;
		}

		// refractive index
		if (part == "Ni")
		{
			StringUtils::readUntilSpace(line, lineIndex, part);
			currentMaterial.refractiveIndex = StringUtils::parseDouble(part);

			continue;
		}

		// ambient map
		if (part == "map_Ka" && !hasAmbientMap)
		{
			ImageTexture imageTexture;
			imageTexture.id = ++currentId;
			currentMaterial.ambientMapTextureId = imageTexture.id;

			StringUtils::readUntilSpace(line, lineIndex, part);
			imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);

			result.textures.push_back(imageTexture);
			hasAmbientMap = true;

			continue;
		}

		// diffuse map
		if (part == "map_Kd" && !hasDiffuseMap)
		{
			ImageTexture imageTexture;
			imageTexture.id = ++currentId;
			currentMaterial.diffuseMapTextureId = imageTexture.id;

			StringUtils::readUntilSpace(line, lineIndex, part);
			imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);

			result.textures.push_back(imageTexture);
			hasDiffuseMap = true;

			continue;
		}

		// specular map
		if (part == "map_Ks" && !hasSpecularMap)
		{
			ImageTexture imageTexture;
			imageTexture.id = ++currentId;
			currentMaterial.specularMapTextureId = imageTexture.id;

			StringUtils::readUntilSpace(line, lineIndex, part);
			imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);

			result.textures.push_back(imageTexture);
			hasSpecularMap = true;

			continue;
		}

		// bump map
		if ((part == "map_bump" || part == "bump") && !hasNormalMap)
		{
			ImageTexture imageTexture;
			imageTexture.id = ++currentId;
			imageTexture.isBumpMap = true;
			currentMaterial.normalMapTextureId = imageTexture.id;

			StringUtils::readUntilSpace(line, lineIndex, part);
			imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);

			result.textures.push_back(imageTexture);
			hasNormalMap = true;

			continue;
		}

		// normal map
		if (part == "map_normal" && !hasNormalMap)
		{
			ImageTexture imageTexture;
			imageTexture.id = ++currentId;
			imageTexture.isNormalMap = true;
			currentMaterial.normalMapTextureId = imageTexture.id;

			StringUtils::readUntilSpace(line, lineIndex, part);
			imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);

			result.textures.push_back(imageTexture);
			hasNormalMap = true;

			continue;
		}

		// mask map
		if (part == "map_d" && !hasMaskMap)
		{
			ImageTexture imageTexture;
			imageTexture.id = ++currentId;
			currentMaterial.maskMapTextureId = imageTexture.id;

			StringUtils::readUntilSpace(line, lineIndex, part);
			imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);

			result.textures.push_back(imageTexture);
			hasMaskMap = true;

			continue;
		}

		// height map
		if (part == "map_height" && !hasHeightMap)
		{
			ImageTexture imageTexture;
			imageTexture.id = ++currentId;
			currentMaterial.heightMapTextureId = imageTexture.id;

			StringUtils::readUntilSpace(line, lineIndex, part);
			imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);

			result.textures.push_back(imageTexture);
			hasHeightMap = true;

			continue;
		}
	}

	file.close();

	if (materialPending)
		result.materials.push_back(currentMaterial);
}

void ObjModelLoader::processFace(const std::string& line, std::vector<Vector3>& vertices, std::vector<Vector3>& normals, std::vector<Vector2>& texcoords, const ModelLoaderInfo& info, ModelLoaderResult& result, PrimitiveGroup &combinedGroup, int& currentId, int currentMaterialId)
{
	Log& log = App::getLog();

	std::vector<int> vertexIndices;
	std::vector<int> normalIndices;
	std::vector<int> texcoordIndices;

	std::string part1;
	int lineIndex1 = 0;

	StringUtils::readUntilSpace(line, lineIndex1, part1);

	// determine what indices are available from the slash count
	int slashCount = (int)std::count(part1.begin(), part1.end(), '/');
	bool doubleSlash = (part1.find("//") != std::string::npos);
	bool hasTexcoords = (slashCount > 0 && !doubleSlash);
	bool hasNormals = (slashCount > 1);

	lineIndex1 = 0;

	while (StringUtils::readUntilSpace(line, lineIndex1, part1))
	{
		std::replace(part1.begin(), part1.end(), '/', ' ');

		std::string part2;
		int lineIndex2 = 0;

		StringUtils::readUntilSpace(part1, lineIndex2, part2);
		int vertexIndex = strtol(part2.c_str(), nullptr, 10);

		if (vertexIndex < 0)
			vertexIndex = (int)vertices.size() + vertexIndex;
		else
			vertexIndex--;

		if (vertexIndex < 0 || vertexIndex >= (int)vertices.size())
		{
			log.logWarning("Vertex index (%s) was out of bounds", vertexIndex);
			return;
		}

		vertexIndices.push_back(vertexIndex);

		if (hasTexcoords)
		{
			StringUtils::readUntilSpace(part1, lineIndex2, part2);
			int texcoordIndex = strtol(part2.c_str(), nullptr, 10);

			if (texcoordIndex < 0)
				texcoordIndex = (int)texcoords.size() + texcoordIndex;
			else
				texcoordIndex--;

			if (texcoordIndex < 0 || texcoordIndex >= (int)texcoords.size())
			{
				log.logWarning("Texcoord index (%s) was out of bounds", texcoordIndex);
				return;
			}

			texcoordIndices.push_back(texcoordIndex);
		}

		if (hasNormals)
		{
			StringUtils::readUntilSpace(part1, lineIndex2, part2);
			int normalIndex = strtol(part2.c_str(), nullptr, 10);

			if (normalIndex < 0)
				normalIndex = (int)normals.size() + normalIndex;
			else
				normalIndex--;

			if (normalIndex < 0 || normalIndex >= (int)normals.size())
			{
				log.logWarning("Normal index (%s) was out of bounds", normalIndex);
				return;
			}

			normalIndices.push_back(normalIndex);
		}
	}

	if (vertexIndices.size() < 3)
	{
		log.logWarning("Too few vertices (%s) in a face", vertexIndices.size());
		return;
	}

	// triangulate
	for (int i = 2; i < (int)vertexIndices.size(); ++i)
	{
		Triangle triangle;
		triangle.id = ++currentId;
		triangle.materialId = currentMaterialId;
		triangle.invisible = info.invisibleTriangles;

		if (info.enableGroups && result.groups.size() > 0)
			result.groups.back().primitiveIds.push_back(triangle.id);

		if (info.enableCombinedGroup)
			combinedGroup.primitiveIds.push_back(triangle.id);

		triangle.vertices[0] = vertices[vertexIndices[0]];
		triangle.vertices[1] = vertices[vertexIndices[i - 1]];
		triangle.vertices[2] = vertices[vertexIndices[i]];

		if (hasNormals)
		{
			triangle.normals[0] = normals[normalIndices[0]];
			triangle.normals[1] = normals[normalIndices[i - 1]];
			triangle.normals[2] = normals[normalIndices[i]];
		}
		else
		{
			Vector3 v0tov1 = triangle.vertices[1] - triangle.vertices[0];
			Vector3 v0tov2 = triangle.vertices[2] - triangle.vertices[0];
			Vector3 normal = v0tov1.cross(v0tov2).normalized();

			triangle.normals[0] = triangle.normals[1] = triangle.normals[2] = normal;
		}

		if (hasTexcoords)
		{
			triangle.texcoords[0] = texcoords[texcoordIndices[0]];
			triangle.texcoords[1] = texcoords[texcoordIndices[i - 1]];
			triangle.texcoords[2] = texcoords[texcoordIndices[i]];
		}

		result.triangles.push_back(triangle);
	}
}
