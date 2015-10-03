// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
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

	size_t findMaterialIndex(const std::vector<Material>& materials, size_t materialId)
	{
		auto result = std::find_if(materials.begin(), materials.end(), [materialId](const Material& material)
		{
			return material.id == materialId;
		});

		if (result == materials.end())
			throw std::runtime_error(tfm::format("Could not find OpenCL material index for material id (%d)", materialId));

		return (result - materials.begin());
	}

	int findTextureIndex(const std::vector<ImageTexture>& textures, size_t textureId)
	{
		auto it = std::find_if(textures.begin(), textures.end(), [textureId](const ImageTexture& texture)
		{
			return texture.id == textureId;
		});

		if (it == textures.end())
			return -1;

		return int((*it).getImagePoolIndex());
	}
}

void CLScene::readSceneFull(const Scene& scene)
{
	Log& log = App::getLog();

	readSceneCamera(scene);

	readColor(raytracer.backgroundColor, scene.raytracer.backgroundColor);
	readColor(raytracer.offLensColor, scene.raytracer.offLensColor);
	raytracer.rayStartOffset = cl_float(scene.raytracer.rayStartOffset);
	raytracer.maxRayIterations = cl_int(scene.raytracer.maxRayIterations);
	raytracer.multiSamples = cl_int(scene.raytracer.multiSamples);
	raytracer.timeSamples = cl_int(scene.raytracer.timeSamples);
	raytracer.cameraSamples = cl_int(scene.raytracer.cameraSamples);

	toneMapper.type = cl_int(scene.toneMapper.type);
	toneMapper.applyGamma = cl_int(scene.toneMapper.applyGamma);
	toneMapper.shouldClamp = cl_int(scene.toneMapper.shouldClamp);
	toneMapper.gamma = cl_float(scene.toneMapper.gamma);
	toneMapper.exposure = cl_float(scene.toneMapper.exposure);
	toneMapper.key = cl_float(scene.toneMapper.key);
	toneMapper.maxLuminance = cl_float(scene.toneMapper.maxLuminance);

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
		readColor(clMaterial.attenuationColor, material.attenuationColor);
		readVector2(clMaterial.texcoordScale, material.texcoordScale);
		clMaterial.shininess = cl_float(material.shininess);
		clMaterial.ambientMapTextureIndex = cl_int(findTextureIndex(scene.textures.imageTextures, material.ambientMapTextureId));
		clMaterial.diffuseMapTextureIndex = cl_int(findTextureIndex(scene.textures.imageTextures, material.diffuseMapTextureId));
		clMaterial.specularMapTextureIndex = cl_int(findTextureIndex(scene.textures.imageTextures, material.specularMapTextureId));
		clMaterial.rayReflectanceMapTextureIndex = cl_int(findTextureIndex(scene.textures.imageTextures, material.rayReflectanceMapTextureId));
		clMaterial.rayTransmittanceMapTextureIndex = cl_int(findTextureIndex(scene.textures.imageTextures, material.rayTransmittanceMapTextureId));
		clMaterial.normalMapTextureIndex = cl_int(findTextureIndex(scene.textures.imageTextures, material.normalMapTextureId));
		clMaterial.maskMapTextureIndex = cl_int(findTextureIndex(scene.textures.imageTextures, material.maskMapTextureId));
		clMaterial.heightMapTextureIndex = cl_int(findTextureIndex(scene.textures.imageTextures, material.heightMapTextureId));
		clMaterial.normalMapType = cl_int(-1);
		clMaterial.skipLighting = cl_int(material.skipLighting);
		clMaterial.nonShadowing = cl_int(material.nonShadowing);
		clMaterial.normalInterpolation = cl_int(material.normalInterpolation);
		clMaterial.invertNormal = cl_int(material.invertNormal);
		clMaterial.fresnelReflection = cl_int(material.fresnelReflection);
		clMaterial.enableAttenuation = cl_int(material.enableAttenuation);
		clMaterial.rayReflectance = cl_float(material.rayReflectance);
		clMaterial.rayTransmittance = cl_float(material.rayTransmittance);
		clMaterial.refractiveIndex = cl_float(material.refractiveIndex);
		clMaterial.attenuationFactor = cl_float(material.attenuationFactor);

		materials.push_back(clMaterial);
	}

	readColor(ambientLight.color, scene.lights.ambientLight.color);
	ambientLight.intensity = cl_float(scene.lights.ambientLight.intensity);
	ambientLight.enableOcclusion = cl_int(scene.lights.ambientLight.enableOcclusion);
	ambientLight.maxOcclusionDistance = cl_float(scene.lights.ambientLight.maxOcclusionDistance);
	ambientLight.occlusionSamplerType = cl_int(scene.lights.ambientLight.occlusionSamplerType);
	ambientLight.occlusionSamples = cl_int(scene.lights.ambientLight.occlusionSamples);
	ambientLight.occlusionSampleDistribution = cl_float(scene.lights.ambientLight.occlusionSampleDistribution);

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
		clLight.radius = cl_float(light.radius);
		clLight.enableSoftShadows = cl_int(light.enableSoftShadows);
		clLight.softShadowSamplerType = cl_int(light.softShadowSamplerType);
		clLight.softShadowSamples = cl_int(light.softShadowSamples);

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
