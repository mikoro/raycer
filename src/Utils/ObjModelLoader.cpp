// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Utils/ModelLoader.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/StringUtils.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Math/Matrix4x4.h"

using namespace Raycer;
using namespace boost::filesystem;

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

	double readDouble(const std::string& input, uint64_t& startIndex, std::string& result)
	{
		StringUtils::readUntilSpace(input, startIndex, result);
		return StringUtils::parseDouble(result);
	}

	void processMaterialFile(const std::string& objFileDirectory, const std::string& mtlFilePath, const ModelLoaderInfo& info, ModelLoaderResult& result, std::map<std::string, uint64_t>& materialsMap, uint64_t& currentId)
	{
		std::string absoluteMtlFilePath = getAbsolutePath(objFileDirectory, mtlFilePath);
		App::getLog().logInfo("Reading MTL file (%s)", absoluteMtlFilePath);
		std::ifstream file(absoluteMtlFilePath);

		if (!file.good())
			throw std::runtime_error("Could not open the MTL file");

		Material currentMaterial = info.baseMaterial;
		bool materialPending = false;

		std::string line;
		std::string part;
		uint64_t lineIndex = 0;

		while (std::getline(file, line))
		{
			part.clear();
			lineIndex = 0;
			StringUtils::readUntilSpace(line, lineIndex, part);

			if (part.size() == 0)
				continue;

			if (part == "newmtl") // new material
			{
				if (materialPending)
					result.materials.push_back(currentMaterial);

				materialPending = true;

				currentMaterial = info.baseMaterial;
				currentMaterial.id = ++currentId;

				StringUtils::readUntilSpace(line, lineIndex, part);
				materialsMap[part] = currentMaterial.id;
			}
			else if (part == "Ka") // ambient reflectance
			{
				currentMaterial.ambientReflectance.r = readDouble(line, lineIndex, part);
				currentMaterial.ambientReflectance.g = readDouble(line, lineIndex, part);
				currentMaterial.ambientReflectance.b = readDouble(line, lineIndex, part);
			}
			else if (part == "Kd") // diffuse reflectance + reflectance
			{
				currentMaterial.diffuseReflectance.r = currentMaterial.reflectance.r = readDouble(line, lineIndex, part);
				currentMaterial.diffuseReflectance.g = currentMaterial.reflectance.g = readDouble(line, lineIndex, part);
				currentMaterial.diffuseReflectance.b = currentMaterial.reflectance.b = readDouble(line, lineIndex, part);
			}
			else if (part == "Ks") // specular reflectance
			{
				currentMaterial.specularReflectance.r = readDouble(line, lineIndex, part);
				currentMaterial.specularReflectance.g = readDouble(line, lineIndex, part);
				currentMaterial.specularReflectance.b = readDouble(line, lineIndex, part);
			}
			else if (part == "Ke") // emittance
			{
				currentMaterial.emittance.r = readDouble(line, lineIndex, part);
				currentMaterial.emittance.g = readDouble(line, lineIndex, part);
				currentMaterial.emittance.b = readDouble(line, lineIndex, part);
			}
			else if (part == "Ns") // shininess
			{
				currentMaterial.shininess = readDouble(line, lineIndex, part);
			}
			else if (part == "Ni") // refractive index
			{
				currentMaterial.refractiveIndex = readDouble(line, lineIndex, part);
			}
			else if (part == "map_Ka" && currentMaterial.ambientMapTextureId == 0) // ambient map
			{
				ImageTexture imageTexture;
				imageTexture.id = ++currentId;
				currentMaterial.ambientMapTextureId = imageTexture.id;

				StringUtils::readUntilSpace(line, lineIndex, part);
				imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);
				imageTexture.applyGamma = !StringUtils::endsWith(imageTexture.imageFilePath, ".hdr");

				result.textures.push_back(imageTexture);
			}
			else if (part == "map_Kd" && currentMaterial.diffuseMapTextureId == 0) // diffuse map + reflectance map
			{
				ImageTexture imageTexture;
				imageTexture.id = ++currentId;
				currentMaterial.diffuseMapTextureId = currentMaterial.reflectanceMapTextureId = imageTexture.id;

				StringUtils::readUntilSpace(line, lineIndex, part);
				imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);
				imageTexture.applyGamma = !StringUtils::endsWith(imageTexture.imageFilePath, ".hdr");

				result.textures.push_back(imageTexture);
			}
			else if (part == "map_Ks" && currentMaterial.specularMapTextureId == 0) // specular map
			{
				ImageTexture imageTexture;
				imageTexture.id = ++currentId;
				currentMaterial.specularMapTextureId = imageTexture.id;

				StringUtils::readUntilSpace(line, lineIndex, part);
				imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);
				imageTexture.applyGamma = !StringUtils::endsWith(imageTexture.imageFilePath, ".hdr");

				result.textures.push_back(imageTexture);
			}
			else if (part == "map_Ke" && currentMaterial.emittanceMapTextureId == 0) // emittance map
			{
				ImageTexture imageTexture;
				imageTexture.id = ++currentId;
				currentMaterial.emittanceMapTextureId = imageTexture.id;

				StringUtils::readUntilSpace(line, lineIndex, part);
				imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);
				imageTexture.applyGamma = !StringUtils::endsWith(imageTexture.imageFilePath, ".hdr");

				result.textures.push_back(imageTexture);
			}
			else if ((part == "map_bump" || part == "bump") && currentMaterial.normalMapTextureId == 0) // bump map
			{
				ImageTexture imageTexture;
				imageTexture.id = ++currentId;
				imageTexture.isBumpMap = true;
				currentMaterial.normalMapTextureId = imageTexture.id;

				StringUtils::readUntilSpace(line, lineIndex, part);
				imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);
				imageTexture.applyGamma = false;

				result.textures.push_back(imageTexture);
			}
			else if (part == "map_normal" && currentMaterial.normalMapTextureId == 0) // normal map
			{
				ImageTexture imageTexture;
				imageTexture.id = ++currentId;
				imageTexture.isNormalMap = true;
				currentMaterial.normalMapTextureId = imageTexture.id;

				StringUtils::readUntilSpace(line, lineIndex, part);
				imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);
				imageTexture.applyGamma = false;

				result.textures.push_back(imageTexture);
			}
			else if (part == "map_d" && currentMaterial.maskMapTextureId == 0) // mask map
			{
				ImageTexture imageTexture;
				imageTexture.id = ++currentId;
				currentMaterial.maskMapTextureId = imageTexture.id;

				StringUtils::readUntilSpace(line, lineIndex, part);
				imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);
				imageTexture.applyGamma = false;

				result.textures.push_back(imageTexture);
			}
			else if (part == "map_height" && currentMaterial.heightMapTextureId == 0) // height map
			{
				ImageTexture imageTexture;
				imageTexture.id = ++currentId;
				currentMaterial.heightMapTextureId = imageTexture.id;

				StringUtils::readUntilSpace(line, lineIndex, part);
				imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, part);
				imageTexture.applyGamma = false;

				result.textures.push_back(imageTexture);
			}
		}

		file.close();

		if (materialPending)
			result.materials.push_back(currentMaterial);
	}

	void processFace(const std::string& line, std::vector<Vector3>& vertices, std::vector<Vector3>& normals, std::vector<Vector2>& texcoords, const ModelLoaderInfo& info, ModelLoaderResult& result, PrimitiveGroup& combinedGroup, uint64_t& currentId, uint64_t currentMaterialId)
	{
		Log& log = App::getLog();

		std::vector<uint64_t> vertexIndices;
		std::vector<uint64_t> normalIndices;
		std::vector<uint64_t> texcoordIndices;

		std::string part1;
		uint64_t lineIndex1 = 0;

		StringUtils::readUntilSpace(line, lineIndex1, part1);

		// determine what indices are available from the slash count
		uint64_t slashCount = std::count(part1.begin(), part1.end(), '/');
		bool doubleSlash = (part1.find("//") != std::string::npos);
		bool hasTexcoords = (slashCount > 0 && !doubleSlash);
		bool hasNormals = (slashCount > 1);

		lineIndex1 = 0;

		while (StringUtils::readUntilSpace(line, lineIndex1, part1))
		{
			std::replace(part1.begin(), part1.end(), '/', ' ');

			std::string part2;
			uint64_t lineIndex2 = 0;

			StringUtils::readUntilSpace(part1, lineIndex2, part2);
			int64_t vertexIndex = strtoll(part2.c_str(), nullptr, 10);

			if (vertexIndex < 0)
				vertexIndex = int64_t(vertices.size()) + vertexIndex;
			else
				vertexIndex--;

			if (vertexIndex < 0 || vertexIndex >= int64_t(vertices.size()))
			{
				log.logWarning("Vertex index (%s) was out of bounds", vertexIndex);
				return;
			}

			vertexIndices.push_back(uint64_t(vertexIndex));

			if (hasTexcoords)
			{
				StringUtils::readUntilSpace(part1, lineIndex2, part2);
				int64_t texcoordIndex = strtoll(part2.c_str(), nullptr, 10);

				if (texcoordIndex < 0)
					texcoordIndex = int64_t(texcoords.size()) + texcoordIndex;
				else
					texcoordIndex--;

				if (texcoordIndex < 0 || texcoordIndex >= int64_t(texcoords.size()))
				{
					log.logWarning("Texcoord index (%s) was out of bounds", texcoordIndex);
					return;
				}

				texcoordIndices.push_back(uint64_t(texcoordIndex));
			}

			if (hasNormals)
			{
				StringUtils::readUntilSpace(part1, lineIndex2, part2);
				int64_t normalIndex = strtoll(part2.c_str(), nullptr, 10);

				if (normalIndex < 0)
					normalIndex = int64_t(normals.size()) + normalIndex;
				else
					normalIndex--;

				if (normalIndex < 0 || normalIndex >= int64_t(normals.size()))
				{
					log.logWarning("Normal index (%s) was out of bounds", normalIndex);
					return;
				}

				normalIndices.push_back(uint64_t(normalIndex));
			}
		}

		if (vertexIndices.size() < 3)
		{
			log.logWarning("Too few vertices (%s) in a face", vertexIndices.size());
			return;
		}

		// triangulate
		for (uint64_t i = 2; i < vertexIndices.size(); ++i)
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
}

ModelLoaderResult ModelLoader::readObjFile(const ModelLoaderInfo& info)
{
	Log& log = App::getLog();

	log.logInfo("Reading OBJ file (%s)", info.modelFilePath);

	auto startTime = std::chrono::high_resolution_clock::now();

	ModelLoaderResult result;

	PrimitiveGroup combinedGroup;
	combinedGroup.id = info.combinedGroupId;
	combinedGroup.invisible = info.invisibleCombinedGroup;

	Instance combinedGroupInstance;
	combinedGroupInstance.id = info.combinedGroupInstanceId;
	combinedGroupInstance.primitiveId = combinedGroup.id;

	uint64_t currentId = info.idStartOffset;
	uint64_t currentMaterialId = info.defaultMaterialId;

	std::map<std::string, uint64_t> materialsMap;
	std::string objFileDirectory = boost::filesystem::absolute(info.modelFilePath).parent_path().string();

	Matrix4x4 scaling = Matrix4x4::scale(info.scale);
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(info.rotate);
	Matrix4x4 translation = Matrix4x4::translate(info.translate);
	Matrix4x4 transformation = translation * rotation * scaling;
	Matrix4x4 transformationInvT = transformation.inverted().transposed();

	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;

	FILE* file = fopen(info.modelFilePath.c_str(), "r");

	if (file == nullptr)
		throw std::runtime_error("Could not open the OBJ file");

	char buffer[1024];
	std::string line;
	std::string part;
	uint64_t lineIndex = 0;

	while (fgets(buffer, sizeof(buffer), file) != nullptr)
	{
		line.assign(buffer);
		part.clear();
		lineIndex = 0;
		StringUtils::readUntilSpace(line, lineIndex, part);

		if (part.size() == 0)
			continue;

		if (part == "mtllib" && !info.ignoreMaterials) // new material file
		{
			StringUtils::readUntilSpace(line, lineIndex, part);
			processMaterialFile(objFileDirectory, part, info, result, materialsMap, currentId);
		}
		else if (part == "g" && info.enableGroups) // new group
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
		else if (part == "usemtl" && !info.ignoreMaterials) // select material
		{
			StringUtils::readUntilSpace(line, lineIndex, part);

			if (materialsMap.count(part))
				currentMaterialId = materialsMap[part];
			else
			{
				log.logWarning("Could not find material named \"%s\"", part);
				currentMaterialId = info.defaultMaterialId;
			}
		}
		else if (part == "v") // vertex
		{
			Vector3 vertex;

			vertex.x = readDouble(line, lineIndex, part);
			vertex.y = readDouble(line, lineIndex, part);
			vertex.z = readDouble(line, lineIndex, part);

			vertices.push_back(transformation.transformPosition(vertex));
		}
		else if (part == "vn") // normal
		{
			Vector3 normal;

			normal.x = readDouble(line, lineIndex, part);
			normal.y = readDouble(line, lineIndex, part);
			normal.z = readDouble(line, lineIndex, part);

			normals.push_back(transformationInvT.transformDirection(normal).normalized());
		}
		else if (part == "vt") // texcoord
		{
			Vector2 texcoord;

			texcoord.x = readDouble(line, lineIndex, part);
			texcoord.y = readDouble(line, lineIndex, part);

			texcoords.push_back(texcoord);
		}
		else if (part == "f") // face
			processFace(line.substr(lineIndex), vertices, normals, texcoords, info, result, combinedGroup, currentId, currentMaterialId);
	}

	fclose(file);

	if (info.enableCombinedGroup)
	{
		result.groups.push_back(combinedGroup);

		if (info.enableCombinedGroupInstance)
			result.instances.push_back(combinedGroupInstance);
	}

	auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();

	log.logInfo("OBJ file reading finished (time: %d ms, groups: %s, triangles: %s, materials: %s, textures: %s)", milliseconds, result.groups.size(), result.triangles.size(), result.materials.size(), result.textures.size());

	return result;
}
