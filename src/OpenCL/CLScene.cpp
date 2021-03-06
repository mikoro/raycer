﻿// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "OpenCL/CLScene.h"
#include "Raytracing/Scene.h"
#include "App.h"
#include "Utils/Log.h"

using namespace Raycer;

namespace
{
	void readVector2(cl_float2& destination, const Vector2& source)
	{
		destination.s[0] = cl_float(source.x);
		destination.s[1] = cl_float(source.y);
	}

	void readVector3(cl_float4& destination, const Vector3& source)
	{
		destination.s[0] = cl_float(source.x);
		destination.s[1] = cl_float(source.y);
		destination.s[2] = cl_float(source.z);
		destination.s[3] = 0.0f;
	}

	void readColor(cl_float4& destination, const Color& source)
	{
		destination.s[0] = cl_float(source.r);
		destination.s[1] = cl_float(source.g);
		destination.s[2] = cl_float(source.b);
		destination.s[3] = cl_float(source.a);
	}

	void readEulerAngle(cl_float4& destination, const EulerAngle& source)
	{
		destination.s[0] = cl_float(source.pitch);
		destination.s[1] = cl_float(source.yaw);
		destination.s[2] = cl_float(source.roll);
		destination.s[3] = 0.0f;
	}

	uint64_t findMaterialIndex(const std::vector<Material>& materials, uint64_t materialId)
	{
		auto result = std::find_if(materials.begin(), materials.end(), [materialId](const Material& material)
		{
			return material.id == materialId;
		});

		if (result == materials.end())
			throw std::runtime_error(tfm::format("Could not find OpenCL material index for material id (%d)", materialId));

		return (result - materials.begin());
	}

	cl_int findTextureIndex(const std::vector<ImageTexture>& textures, uint64_t textureId)
	{
		auto it = std::find_if(textures.begin(), textures.end(), [textureId](const ImageTexture& texture)
		{
			return texture.id == textureId;
		});

		if (it == textures.end())
			return -1;

		return cl_int((*it).getImagePoolIndex());
	}
}

void CLScene::readSceneFull(const Scene& scene)
{
	Log& log = App::getLog();

	readColor(general.backgroundColor, scene.general.backgroundColor);
	readColor(general.offLensColor, scene.general.offLensColor);
	general.rayStartOffset = cl_float(scene.general.rayStartOffset);
	general.maxRayIterations = cl_int(scene.general.maxRayIterations);
	general.maxPathLength = cl_int(scene.general.maxPathLength);
	general.pathSampleCount = cl_int(scene.general.pathSampleCount);
	general.multiSampleCountSqrt = cl_int(scene.general.multiSampleCountSqrt);
	general.timeSampleCount = cl_int(scene.general.timeSampleCount);
	general.cameraSampleCountSqrt = cl_int(scene.general.cameraSampleCountSqrt);
	general.visualizeDepth = cl_int(scene.general.visualizeDepth);
	general.visualizeDepthMaxDistance = cl_float(scene.general.visualizeDepthMaxDistance);
	general.enableNormalMapping = cl_int(scene.general.enableNormalMapping);

	readSceneCamera(scene);

	toneMapper.type = cl_int(scene.toneMapper.type);
	toneMapper.applyGamma = cl_int(scene.toneMapper.applyGamma);
	toneMapper.shouldClamp = cl_int(scene.toneMapper.shouldClamp);
	toneMapper.gamma = cl_float(scene.toneMapper.gamma);
	toneMapper.exposure = cl_float(scene.toneMapper.exposure);
	toneMapper.key = cl_float(scene.toneMapper.key);
	toneMapper.enableAveraging = cl_int(scene.toneMapper.enableAveraging);
	toneMapper.averagingAlpha = cl_float(scene.toneMapper.averagingAlpha);

	readColor(simpleFog.color, scene.simpleFog.color);
	simpleFog.enabled = cl_int(scene.simpleFog.enabled);
	simpleFog.distance = cl_float(scene.simpleFog.distance);
	simpleFog.steepness = cl_float(scene.simpleFog.steepness);
	simpleFog.heightDispersion = cl_int(scene.simpleFog.heightDispersion);
	simpleFog.height = cl_float(scene.simpleFog.height);
	simpleFog.heightSteepness = cl_float(scene.simpleFog.heightSteepness);

	materials.clear();

	for (const Material& material : scene.materials)
	{
		OpenCL::Material clMaterial;

		readColor(clMaterial.ambientReflectance, material.ambientReflectance);
		readColor(clMaterial.diffuseReflectance, material.diffuseReflectance);
		readColor(clMaterial.specularReflectance, material.specularReflectance);
		readColor(clMaterial.emittance, material.emittance);
		readColor(clMaterial.rayTransmissionAttenuationColor, material.rayTransmissionAttenuationColor);
		readVector2(clMaterial.texcoordScale, material.texcoordScale);
		clMaterial.skipLighting = cl_int(material.skipLighting);
		clMaterial.nonShadowing = cl_int(material.nonShadowing);
		clMaterial.normalInterpolation = cl_int(material.normalInterpolation);
		clMaterial.invertNormal = cl_int(material.invertNormal);
		clMaterial.isEmissive = cl_int(material.isEmissive);
		clMaterial.ambientMapTextureIndex = findTextureIndex(scene.textures.imageTextures, material.ambientMapTextureId);
		clMaterial.diffuseMapTextureIndex = findTextureIndex(scene.textures.imageTextures, material.diffuseMapTextureId);
		clMaterial.specularShininess = cl_float(material.specularShininess);
		clMaterial.specularMapTextureIndex = findTextureIndex(scene.textures.imageTextures, material.specularMapTextureId);
		clMaterial.emittanceMapTextureIndex = findTextureIndex(scene.textures.imageTextures, material.emittanceMapTextureId);
		clMaterial.fresnelReflection = cl_int(material.fresnelReflection);
		clMaterial.refractiveIndex = cl_float(material.refractiveIndex);
		clMaterial.rayReflectance = cl_float(material.rayReflectance);
		clMaterial.rayReflectanceMapTextureIndex = findTextureIndex(scene.textures.imageTextures, material.rayReflectanceMapTextureId);
		clMaterial.rayReflectanceGlossinessSampleCountSqrt = cl_int(material.rayReflectanceGlossinessSampleCountSqrt);
		clMaterial.rayReflectanceGlossiness = cl_float(material.rayReflectanceGlossiness);
		clMaterial.rayTransmittance = cl_float(material.rayTransmittance);
		clMaterial.rayTransmittanceMapTextureIndex = findTextureIndex(scene.textures.imageTextures, material.rayTransmittanceMapTextureId);
		clMaterial.rayTransmittanceGlossinessSampleCountSqrt = cl_int(material.rayTransmittanceGlossinessSampleCountSqrt);
		clMaterial.rayTransmittanceGlossiness = cl_float(material.rayTransmittanceGlossiness);
		clMaterial.enableRayTransmissionAttenuation = cl_int(material.enableRayTransmissionAttenuation);
		clMaterial.rayTransmissionAttenuationFactor = cl_float(material.rayTransmissionAttenuationFactor);
		clMaterial.normalMapTextureIndex = findTextureIndex(scene.textures.imageTextures, material.normalMapTextureId);
		clMaterial.maskMapTextureIndex = findTextureIndex(scene.textures.imageTextures, material.maskMapTextureId);
		clMaterial.heightMapTextureIndex = findTextureIndex(scene.textures.imageTextures, material.heightMapTextureId);
		
		materials.push_back(clMaterial);
	}

	readColor(ambientLight.color, scene.lights.ambientLight.color);
	ambientLight.intensity = cl_float(scene.lights.ambientLight.intensity);
	ambientLight.enableAmbientOcclusion = cl_int(scene.lights.ambientLight.enableAmbientOcclusion);
	ambientLight.ambientOcclusionSampleCountSqrt = cl_int(scene.lights.ambientLight.ambientOcclusionSampleCountSqrt);
	ambientLight.ambientOcclusionMaxSampleDistance = cl_float(scene.lights.ambientLight.ambientOcclusionMaxSampleDistance);
	ambientLight.ambientOcclusionSampleDistribution = cl_float(scene.lights.ambientLight.ambientOcclusionSampleDistribution);

	directionalLights.clear();

	for (const DirectionalLight& light : scene.lights.directionalLights)
	{
		OpenCL::DirectionalLight clLight;

		readColor(clLight.color, light.color);
		readVector3(clLight.direction, light.direction);
		clLight.intensity = cl_float(light.intensity);

		directionalLights.push_back(clLight);
	}

	state.directionalLightCount = cl_int(directionalLights.size());

	pointLights.clear();

	for (const PointLight& light : scene.lights.pointLights)
	{
		OpenCL::PointLight clLight;

		readColor(clLight.color, light.color);
		readVector3(clLight.position, light.position);
		clLight.intensity = cl_float(light.intensity);
		clLight.maxDistance = cl_float(light.maxDistance);
		clLight.attenuation = cl_float(light.attenuation);
		clLight.enableAreaLight = cl_int(light.enableAreaLight);
		clLight.areaLightSampleCountSqrt = cl_int(light.areaLightSampleCountSqrt);
		clLight.areaLightRadius = cl_float(light.areaLightRadius);

		pointLights.push_back(clLight);
	}

	state.pointLightCount = cl_int(pointLights.size());

	triangles.clear();

	if (scene.rootBVH.bvh.orderedPrimitives.size() == 0)
		log.logWarning("Root BVH is empty so nothing will get rendered when using OpenCL");

	for (Primitive* primitivePtr : scene.rootBVH.bvh.orderedPrimitives)
	{
		Triangle* triangle = dynamic_cast<Triangle*>(primitivePtr);

		if (triangle == nullptr)
			throw std::runtime_error("When using OpenCL, the root BVH must only contain triangles");

		OpenCL::Triangle clTriangle;

		readVector3(clTriangle.vertices[0], triangle->vertices[0]);
		readVector3(clTriangle.vertices[1], triangle->vertices[1]);
		readVector3(clTriangle.vertices[2], triangle->vertices[2]);
		readVector3(clTriangle.normals[0], triangle->normals[0]);
		readVector3(clTriangle.normals[1], triangle->normals[1]);
		readVector3(clTriangle.normals[2], triangle->normals[2]);
		readVector2(clTriangle.texcoords[0], triangle->texcoords[0]);
		readVector2(clTriangle.texcoords[1], triangle->texcoords[1]);
		readVector2(clTriangle.texcoords[2], triangle->texcoords[2]);
		readVector3(clTriangle.normal, triangle->normal);
		readVector3(clTriangle.tangent, triangle->tangent);
		readVector3(clTriangle.bitangent, triangle->bitangent);
		clTriangle.materialIndex = cl_uint(findMaterialIndex(scene.materials, triangle->materialId));

		triangles.push_back(clTriangle);
	}

	state.triangleCount = cl_int(triangles.size());

	bvhNodes.clear();

	for (const FlatBVHNode& node : scene.rootBVH.bvh.flatNodes)
	{
		OpenCL::BVHNode clNode;

		readVector3(clNode.aabb.min, node.aabb.getMin());
		readVector3(clNode.aabb.max, node.aabb.getMax());
		clNode.rightOffset = cl_int(node.rightOffset);
		clNode.startOffset = cl_int(node.startOffset);
		clNode.primitiveCount = cl_int(node.primitiveCount);

		bvhNodes.push_back(clNode);
	}

	state.bvhNodeCount = cl_int(bvhNodes.size());
}

void CLScene::readSceneCamera(const Scene& scene)
{
	readVector3(camera.position, scene.camera.cameraState.position);
	readVector3(camera.forward, scene.camera.cameraState.forward);
	readVector3(camera.right, scene.camera.cameraState.right);
	readVector3(camera.up, scene.camera.cameraState.up);
	readVector3(camera.imagePlaneCenter, scene.camera.cameraState.imagePlaneCenter);
	readVector3(camera.translateInTime, scene.camera.translateInTime);
	readEulerAngle(camera.rotateInTime, scene.camera.rotateInTime);
	camera.projectionType = cl_int(scene.camera.projectionType);
	camera.isTimeVariant = cl_int(scene.camera.isTimeVariant);
	camera.hasMoved = cl_int(scene.camera.cameraHasMoved);
	camera.fov = cl_float(scene.camera.fov);
	camera.orthoSize = cl_float(scene.camera.orthoSize);
	camera.fishEyeAngle = cl_float(scene.camera.fishEyeAngle);
	camera.apertureSize = cl_float(scene.camera.apertureSize);
	camera.focalDistance = cl_float(scene.camera.focalDistance);
	camera.aspectRatio = cl_float(scene.camera.aspectRatio);
	camera.imagePlaneDistance = cl_float(scene.camera.imagePlaneDistance);
	camera.imagePlaneWidth = cl_float(scene.camera.imagePlaneWidth);
	camera.imagePlaneHeight = cl_float(scene.camera.imagePlaneHeight);
}
