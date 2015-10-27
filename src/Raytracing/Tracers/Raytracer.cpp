// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Tracers/Raytracer.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/Material.h"
#include "Raytracing/Lights.h"
#include "Raytracing/Primitives/Primitive.h"
#include "Raytracing/Textures/Texture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Math/ONB.h"

using namespace Raycer;

Color Raytracer::trace(const Scene& scene, const Ray& ray, std::mt19937& generator, const std::atomic<bool>& interrupted)
{
	Intersection intersection;
	return traceRecursive(scene, ray, intersection, 0, generator, interrupted);
}

Color Raytracer::traceRecursive(const Scene& scene, const Ray& ray, Intersection& intersection, uint64_t iteration, std::mt19937& generator, const std::atomic<bool>& interrupted)
{
	Color finalColor = scene.general.backgroundColor;

	if (interrupted)
		return finalColor;

	std::vector<Intersection> intersections;

	for (Primitive* primitive : scene.primitives.visible)
	{
		intersections.clear();
		primitive->intersect(ray, intersection, intersections);
	}

	if (!intersection.wasFound)
		return finalColor;

	if (scene.general.visualizeDepth)
	{
		double depth = 1.0 - std::min(intersection.distance, scene.general.visualizeDepthMaxDistance) / scene.general.visualizeDepthMaxDistance;
		depth = pow(depth, 2.0);
		return Color(depth, depth, depth);
	}

	Material* material = intersection.primitive->material;

	if (material->skipLighting)
	{
		if (material->ambientMapTexture != nullptr)
			finalColor = material->ambientMapTexture->getColor(intersection.texcoord, intersection.position) * material->ambientMapTexture->intensity;
		else if (material->diffuseMapTexture != nullptr)
			finalColor = material->diffuseMapTexture->getColor(intersection.texcoord, intersection.position) * material->diffuseMapTexture->intensity;

		bool isOutside = (-(ray.direction.dot(intersection.normal)) >= 0.0);

		if (scene.simpleFog.enabled && isOutside)
			finalColor = calculateSimpleFogColor(scene, intersection, finalColor);

		return finalColor;
	}

	if (material->normalMapTexture != nullptr)
	{
		TextureNormalDataType normalType = TextureNormalDataType::NONE;
		Vector3 normalData = material->normalMapTexture->getNormalData(intersection.texcoord, intersection.position, normalType);

		if (normalType == TextureNormalDataType::BUMP_MAP)
		{
			normalData *= material->normalMapTexture->intensity;
			ONB& onb = intersection.onb;
			Vector3 bumpMapNormal = onb.w + normalData.x * (onb.u.cross(onb.w)) + normalData.y * (onb.v.cross(onb.w));
			intersection.normal = bumpMapNormal.normalized();
		}
		else if (normalType == TextureNormalDataType::NORMAL_MAP)
		{
			ONB& onb = intersection.onb;
			Vector3 normalMapNormal = onb.u * normalData.x + onb.v * normalData.y + onb.w * normalData.z;
			intersection.normal = normalMapNormal.normalized();
		}
		else if (normalType == TextureNormalDataType::GRADIENT)
		{
			Vector3 adjustedNormal = (intersection.normal - (normalData * material->normalMapTexture->intensity));
			intersection.normal = adjustedNormal.normalized();
		}
	}

	double c1 = -(ray.direction.dot(intersection.normal));
	bool isOutside = (c1 >= 0.0);
	double n1 = isOutside ? 1.0 : material->refractiveIndex;
	double n2 = isOutside ? material->refractiveIndex : 1.0;
	double fresnelReflectance = 1.0;
	double fresnelTransmittance = 1.0;

	if (material->fresnelReflection)
	{
		double rf0 = (n2 - n1) / (n2 + n1);
		rf0 = rf0 * rf0;
		fresnelReflectance = rf0 + (1.0 - rf0) * pow(1.0 - std::abs(c1), 5.0);
		fresnelTransmittance = 1.0 - fresnelReflectance;
	}

	double mappedReflectance = 1.0;
	double mappedTransmittance = 1.0;

	if (material->rayReflectanceMapTexture != nullptr)
		mappedReflectance = material->rayReflectanceMapTexture->getValue(intersection.texcoord, intersection.position);

	if (material->rayTransmittanceMapTexture != nullptr)
		mappedTransmittance = material->rayTransmittanceMapTexture->getValue(intersection.texcoord, intersection.position);

	double totalReflectance = material->rayReflectance * mappedReflectance * fresnelReflectance;
	double totalTransmittance = material->rayTransmittance * mappedTransmittance * fresnelTransmittance;

	Color reflectedColor;

	// calculate and trace reflected ray
	if (totalReflectance > 0.0 && iteration < scene.general.maxRayIterations)
	{
		Vector3 R = ray.direction + 2.0 * c1 * intersection.normal;
		R.normalize();

		Ray reflectedRay;
		Intersection reflectedIntersection;

		reflectedRay.origin = intersection.position + R * scene.general.rayStartOffset;
		reflectedRay.direction = R;
		reflectedRay.precalculate();

		reflectedColor = traceRecursive(scene, reflectedRay, reflectedIntersection, iteration + 1, generator, interrupted) * totalReflectance;

		// only attenuate if ray has traveled inside a primitive
		if (!isOutside && reflectedIntersection.wasFound && material->enableAttenuation)
		{
			double a = exp(-material->attenuationFactor * reflectedIntersection.distance);
			reflectedColor = Color::lerp(material->attenuationColor, reflectedColor, a);
		}
	}

	Color transmittedColor;

	// calculate and trace refracted ray
	if (totalTransmittance > 0.0 && iteration < scene.general.maxRayIterations)
	{
		double n = n1 / n2;
		double c2 = 1.0 - (n * n) * (1.0 - c1 * c1);

		// no total internal reflection
		if (c2 > 0.0)
		{
			Vector3 T = ray.direction * n + (std::abs(c1) * n - sqrt(c2)) * intersection.normal;
			T.normalize();

			Ray refractedRay;
			Intersection refractedIntersection;

			refractedRay.origin = intersection.position + T * scene.general.rayStartOffset;
			refractedRay.direction = T;
			refractedRay.precalculate();

			transmittedColor = traceRecursive(scene, refractedRay, refractedIntersection, iteration + 1, generator, interrupted) * totalTransmittance;

			// only attenuate if ray has traveled inside a primitive
			if (isOutside && refractedIntersection.wasFound && material->enableAttenuation)
			{
				double a = exp(-material->attenuationFactor * refractedIntersection.distance);
				transmittedColor = Color::lerp(material->attenuationColor, transmittedColor, a);
			}
		}
	}

	double ambientOcclusionAmount = 1.0;

	if (scene.lights.ambientLight.enableOcclusion)
		ambientOcclusionAmount = calculateAmbientOcclusionAmount(scene, intersection, generator);

	Color lightColor = calculateLightColor(scene, ray, intersection, ambientOcclusionAmount, generator);
	finalColor = lightColor + reflectedColor + transmittedColor;

	if (scene.simpleFog.enabled && isOutside)
		finalColor = calculateSimpleFogColor(scene, intersection, finalColor);

	return finalColor;
}

Color Raytracer::calculateLightColor(const Scene& scene, const Ray& ray, const Intersection& intersection, double ambientOcclusionAmount, std::mt19937& generator)
{
	Color lightColor;
	Vector3 directionToCamera = -ray.direction;
	Material* material = intersection.primitive->material;

	Color mappedAmbientReflectance = Color(1.0, 1.0, 1.0);
	Color mappedDiffuseReflectance = Color(1.0, 1.0, 1.0);
	Color mappedSpecularReflectance = Color(1.0, 1.0, 1.0);

	if (material->ambientMapTexture != nullptr)
		mappedAmbientReflectance = material->ambientMapTexture->getColor(intersection.texcoord, intersection.position) * material->ambientMapTexture->intensity;

	if (material->diffuseMapTexture != nullptr)
		mappedDiffuseReflectance = material->diffuseMapTexture->getColor(intersection.texcoord, intersection.position) * material->diffuseMapTexture->intensity;

	if (material->specularMapTexture != nullptr)
		mappedSpecularReflectance = material->specularMapTexture->getColor(intersection.texcoord, intersection.position) * material->specularMapTexture->intensity;

	Color finalAmbientReflectance = material->ambientReflectance * mappedAmbientReflectance;
	Color finalDiffuseReflectance = material->diffuseReflectance * mappedDiffuseReflectance;
	Color finalSpecularReflectance = material->specularReflectance * mappedSpecularReflectance;

	lightColor += scene.lights.ambientLight.color * scene.lights.ambientLight.intensity * ambientOcclusionAmount * finalAmbientReflectance;

	for (const DirectionalLight& light : scene.lights.directionalLights)
	{
		Color directionalLightColor = calculatePhongShadingColor(intersection.normal, -light.direction, directionToCamera, light, finalDiffuseReflectance, finalSpecularReflectance, material->shininess);
		double shadowAmount = calculateShadowAmount(scene, ray, intersection, light);
		lightColor += directionalLightColor * (1.0 - shadowAmount);
	}

	for (const PointLight& light : scene.lights.pointLights)
	{
		Vector3 directionToLight = (light.position - intersection.position);
		double distanceToLight = directionToLight.length();
		directionToLight.normalize();

		Color pointLightColor = calculatePhongShadingColor(intersection.normal, directionToLight, directionToCamera, light, finalDiffuseReflectance, finalSpecularReflectance, material->shininess);
		double shadowAmount = calculateShadowAmount(scene, ray, intersection, light, generator);
		double distanceAttenuation = std::min(1.0, distanceToLight / light.maxDistance);
		distanceAttenuation = 1.0 - pow(distanceAttenuation, light.attenuation);

		lightColor += pointLightColor * distanceAttenuation * (1.0 - shadowAmount);
	}

	for (const SpotLight& light : scene.lights.spotLights)
	{
		Vector3 directionToLight = (light.position - intersection.position);
		double distanceToLight = directionToLight.length();
		directionToLight.normalize();

		Color spotLightColor = calculatePhongShadingColor(intersection.normal, directionToLight, directionToCamera, light, finalDiffuseReflectance, finalSpecularReflectance, material->shininess);
		double shadowAmount = calculateShadowAmount(scene, ray, intersection, light, generator);
		double distanceAttenuation = std::min(1.0, distanceToLight / light.maxDistance);
		distanceAttenuation = 1.0 - pow(distanceAttenuation, light.attenuation);
		double sideAttenuation = light.direction.dot(-directionToLight);

		if (sideAttenuation > 0.0)
		{
			sideAttenuation = std::min(1.0, (1.0 - sideAttenuation) / (light.angle / 180.0));
			sideAttenuation = 1.0 - pow(sideAttenuation, light.sideAttenuation);
			lightColor += spotLightColor * distanceAttenuation * sideAttenuation * (1.0 - shadowAmount);
		}
	}

	return lightColor;
}

Color Raytracer::calculatePhongShadingColor(const Vector3& normal, const Vector3& directionToLight, const Vector3& directionToCamera, const Light& light, const Color& diffuseReflectance, const Color& specularReflectance, double shininess)
{
	Color phongColor;

	double diffuseAmount = directionToLight.dot(normal);

	if (diffuseAmount > 0.0)
	{
		phongColor = light.color * light.intensity * diffuseAmount * diffuseReflectance;

		if (!specularReflectance.isZero())
		{
			Vector3 reflectionDirection = ((2.0 * diffuseAmount * normal) - directionToLight).normalized();
			double specularAmount = reflectionDirection.dot(directionToCamera);

			if (specularAmount > 0.0)
				phongColor += light.color * light.intensity * pow(specularAmount, shininess) * specularReflectance;
		}
	}

	return phongColor;
}

Color Raytracer::calculateSimpleFogColor(const Scene& scene, const Intersection& intersection, const Color& pixelColor)
{
	double t1 = intersection.distance / scene.simpleFog.distance;
	t1 = std::max(0.0, std::min(t1, 1.0));
	t1 = pow(t1, scene.simpleFog.steepness);

	if (scene.simpleFog.heightDispersion && intersection.position.y > 0.0)
	{
		double t2 = intersection.position.y / scene.simpleFog.height;
		t2 = std::max(0.0, std::min(t2, 1.0));
		t2 = pow(t2, scene.simpleFog.heightSteepness);
		t2 = 1.0 - t2;
		t1 *= t2;
	}

	return Color::lerp(pixelColor, scene.simpleFog.color, t1);
}

double Raytracer::calculateAmbientOcclusionAmount(const Scene& scene, const Intersection& intersection, std::mt19937& generator)
{
	Sampler* sampler = samplers[scene.lights.ambientLight.occlusionSamplerType].get();
	std::uniform_int_distribution<uint64_t> randomPermutation;
	uint64_t permutation = randomPermutation(generator);

	double ambientOcclusion = 0.0;
	double distribution = scene.lights.ambientLight.occlusionSampleDistribution;
	uint64_t n = scene.lights.ambientLight.occlusionSamples;

	for (uint64_t y = 0; y < n; ++y)
	{
		for (uint64_t x = 0; x < n; ++x)
		{
			Vector3 sampleDirection = sampler->getHemisphereSample(intersection.onb, distribution, x, y, n, n, permutation, generator);

			Ray sampleRay;
			Intersection sampleIntersection;
			std::vector<Intersection> sampleIntersections;

			sampleRay.origin = intersection.position + sampleDirection * scene.general.rayStartOffset;
			sampleRay.direction = sampleDirection;
			sampleRay.fastOcclusion = true;
			sampleRay.maxDistance = scene.lights.ambientLight.maxOcclusionDistance;
			sampleRay.precalculate();

			for (Primitive* primitive : scene.primitives.visible)
			{
				sampleIntersections.clear();

				if (primitive->intersect(sampleRay, sampleIntersection, sampleIntersections))
				{
					ambientOcclusion += 1.0;
					break;
				}
			}
		}
	}

	return 1.0 - (ambientOcclusion / (double(n) * n));
}

double Raytracer::calculateShadowAmount(const Scene& scene, const Ray& ray, const Intersection& intersection, const DirectionalLight& light)
{
	Vector3 directionToLight = -light.direction;

	Ray shadowRay;
	Intersection shadowIntersection;
	std::vector<Intersection> shadowIntersections;

	shadowRay.origin = intersection.position + directionToLight * scene.general.rayStartOffset;
	shadowRay.direction = directionToLight;
	shadowRay.isShadowRay = true;
	shadowRay.fastOcclusion = true;
	shadowRay.maxDistance = std::numeric_limits<double>::max();
	shadowRay.time = ray.time;
	shadowRay.precalculate();

	for (Primitive* primitive : scene.primitives.visible)
	{
		shadowIntersections.clear();

		if (primitive->intersect(shadowRay, shadowIntersection, shadowIntersections))
			return 1.0;
	}

	return 0.0;
}

double Raytracer::calculateShadowAmount(const Scene& scene, const Ray& ray, const Intersection& intersection, const PointLight& light, std::mt19937& generator)
{
	Vector3 directionToLight = (light.position - intersection.position).normalized();

	if (!light.enableSoftShadows)
	{
		Ray shadowRay;
		Intersection shadowIntersection;
		std::vector<Intersection> shadowIntersections;

		shadowRay.origin = intersection.position + directionToLight * scene.general.rayStartOffset;
		shadowRay.direction = directionToLight;
		shadowRay.isShadowRay = true;
		shadowRay.fastOcclusion = true;
		shadowRay.maxDistance = (light.position - intersection.position).length();
		shadowRay.time = ray.time;
		shadowRay.precalculate();

		for (Primitive* primitive : scene.primitives.visible)
		{
			shadowIntersections.clear();

			if (primitive->intersect(shadowRay, shadowIntersection, shadowIntersections))
				return 1.0;
		}

		return 0.0;
	}

	Vector3 lightRight = directionToLight.cross(Vector3::ALMOST_UP).normalized();
	Vector3 lightUp = lightRight.cross(directionToLight).normalized();

	Sampler* sampler = samplers[light.softShadowSamplerType].get();
	std::uniform_int_distribution<uint64_t> randomPermutation;
	uint64_t permutation = randomPermutation(generator);

	double shadowAmount = 0.0;
	uint64_t n = light.softShadowSamples;

	for (uint64_t y = 0; y < n; ++y)
	{
		for (uint64_t x = 0; x < n; ++x)
		{
			Vector2 jitter = sampler->getDiscSample(x, y, n, n, permutation, generator) * light.radius;
			Vector3 newLightPosition = light.position + jitter.x * lightRight + jitter.y * lightUp;
			Vector3 newDirectionToLight = (newLightPosition - intersection.position).normalized();

			Ray shadowRay;
			Intersection shadowIntersection;
			std::vector<Intersection> shadowIntersections;

			shadowRay.origin = intersection.position + newDirectionToLight * scene.general.rayStartOffset;
			shadowRay.direction = newDirectionToLight;
			shadowRay.isShadowRay = true;
			shadowRay.fastOcclusion = true;
			shadowRay.maxDistance = (newLightPosition - intersection.position).length();
			shadowRay.time = ray.time;
			shadowRay.precalculate();

			for (Primitive* primitive : scene.primitives.visible)
			{
				shadowIntersections.clear();

				if (primitive->intersect(shadowRay, shadowIntersection, shadowIntersections))
				{
					shadowAmount += 1.0;
					break;
				}
			}
		}
	}

	return shadowAmount / (double(n) * n);
}
