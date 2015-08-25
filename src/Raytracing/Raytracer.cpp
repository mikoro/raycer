// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>

#include "Raytracing/Raytracer.h"
#include "Raytracing/RaytracerState.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/Material.h"
#include "Raytracing/Camera.h"
#include "Raytracing/Lights.h"
#include "Raytracing/Primitives/Primitive.h"
#include "Raytracing/Textures/Texture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Rendering/Image.h"
#include "Rendering/Samplers/RandomSampler.h"
#include "Rendering/Samplers/RegularSampler.h"
#include "Rendering/Samplers/JitteredSampler.h"
#include "Rendering/Samplers/CMJSampler.h"
#include "Rendering/Filters/BoxFilter.h"
#include "Rendering/Filters/TentFilter.h"
#include "Rendering/Filters/BellFilter.h"
#include "Rendering/Filters/MitchellFilter.h"
#include "Rendering/Filters/GaussianFilter.h"
#include "Rendering/Filters/LanczosSincFilter.h"
#include "Rendering/ToneMappers/LinearToneMapper.h"
#include "Rendering/ToneMappers/ReinhardToneMapper.h"

using namespace Raycer;

Raytracer::Raytracer()
{
	std::random_device rd;
	generator.seed(rd());

	samplers[SamplerType::RANDOM] = std::make_shared<RandomSampler>();
	samplers[SamplerType::REGULAR] = std::make_shared<RegularSampler>();
	samplers[SamplerType::JITTERED] = std::make_shared<JitteredSampler>();
	samplers[SamplerType::CMJ] = std::make_shared<CMJSampler>();

	filters[FilterType::BOX] = std::make_shared<BoxFilter>();
	filters[FilterType::TENT] = std::make_shared<TentFilter>();
	filters[FilterType::BELL] = std::make_shared<BellFilter>();
	filters[FilterType::MITCHELL] = std::make_shared<MitchellFilter>();
	filters[FilterType::GAUSSIAN] = std::make_shared<GaussianFilter>();
	filters[FilterType::LANCZOS_SINC] = std::make_shared<LanczosSincFilter>();

	toneMappers[ToneMapperType::LINEAR] = std::make_shared<LinearToneMapper>();
	toneMappers[ToneMapperType::REINHARD] = std::make_shared<ReinhardToneMapper>();
}

void Raytracer::run(RaytracerState& state, std::atomic<bool>& interrupted)
{
	Image& image = *state.image;
	Scene& scene = *state.scene;

	#pragma omp parallel for schedule(dynamic, 1000)
	for (int pixelIndex = 0; pixelIndex < state.pixelCount; ++pixelIndex)
	{
		if (interrupted)
			continue;

		int pixelOffsetIndex = pixelIndex + state.pixelOffset;
		double x = (double)(pixelOffsetIndex % state.sceneWidth);
		double y = (double)(pixelOffsetIndex / state.sceneWidth);
		Vector2 pixelCoordinate = Vector2(x, y);

		Color pixelColor = generateMultiSamples(scene, pixelCoordinate, interrupted);
		image.setPixel(pixelIndex, pixelColor);

		// progress reporting to another thread
		if ((pixelIndex + 1) % 100 == 0)
			state.pixelsProcessed += 100;
	}

	if (!interrupted)
		state.pixelsProcessed = state.pixelCount;

	if (scene.toneMapper.type != ToneMapperType::NONE)
	{
		ToneMapper* toneMapper = toneMappers[scene.toneMapper.type].get();
		toneMapper->apply(scene, image);
	}
}

Color Raytracer::generateMultiSamples(const Scene& scene, const Vector2& pixelCoordinate, const std::atomic<bool>& interrupted)
{
	if (scene.raytracer.multiSamples == 0)
		return generateDofSamples(scene, pixelCoordinate, interrupted);

	Color sampledPixelColor;
	int permutation = randomDist(generator);
	int n = scene.raytracer.multiSamples;
	Sampler* sampler = samplers[scene.raytracer.multiSamplerType].get();
	Filter* filter = filters[scene.raytracer.multiSamplerFilterType].get();
	double filterWeightSum = 0.0;
	double filterWidth = filter->getWidth();

	for (int y = 0; y < n; ++y)
	{
		for (int x = 0; x < n; ++x)
		{
			Vector2 sampleOffset = (sampler->getSquareSample(x, y, n, n, permutation) - Vector2(0.5, 0.5)) * 2.0 * filterWidth;
			double filterWeight = filter->getWeight(sampleOffset);
			sampledPixelColor += generateDofSamples(scene, pixelCoordinate + sampleOffset, interrupted) * filterWeight;
			filterWeightSum += filterWeight;
		}
	}

	return sampledPixelColor / filterWeightSum;
}

Color Raytracer::generateDofSamples(const Scene& scene, const Vector2& pixelCoordinate, const std::atomic<bool>& interrupted)
{
	Ray ray = scene.camera.getRay(pixelCoordinate);

	if (ray.isInvalid && scene.raytracer.dofSamples == 0)
		return scene.raytracer.offLensColor;

	if (scene.raytracer.dofSamples == 0)
		return generateTimeSamples(scene, ray, interrupted);

	Color sampledPixelColor;
	int permutation = randomDist(generator);
	int n = scene.raytracer.dofSamples;
	double apertureSize = scene.camera.apertureSize;
	double focalLength = scene.camera.focalLenght;
	Sampler* sampler = samplers[scene.raytracer.dofSamplerType].get();

	Vector3 cameraOrigin = scene.camera.position;
	Vector3 cameraRight = scene.camera.right;
	Vector3 cameraUp = scene.camera.up;

	for (int y = 0; y < n; ++y)
	{
		for (int x = 0; x < n; ++x)
		{
			Vector2 jitter = (sampler->getSquareSample(x, y, n, n, permutation) - Vector2(0.5, 0.5)) * 2.0;
			Ray primaryRay = scene.camera.getRay(pixelCoordinate + jitter);

			if (primaryRay.isInvalid)
			{
				sampledPixelColor += scene.raytracer.offLensColor;
				continue;
			}

			Vector3 focalPoint = primaryRay.origin + primaryRay.direction * focalLength;
			Vector2 diskCoordinate = sampler->getDiskSample(x, y, n, n, permutation);

			Ray sampleRay;
			sampleRay.origin = cameraOrigin + ((diskCoordinate.x * apertureSize) * cameraRight + (diskCoordinate.y * apertureSize) * cameraUp);
			sampleRay.direction = (focalPoint - sampleRay.origin).normalized();
			sampleRay.update();

			sampledPixelColor += generateTimeSamples(scene, sampleRay, interrupted);
		}
	}

	return sampledPixelColor / (double)(n * n);
}

Color Raytracer::generateTimeSamples(const Scene& scene, Ray& ray, const std::atomic<bool>& interrupted)
{
	Intersection intersection;

	if (scene.raytracer.timeSamples == 0)
		return raytrace(scene, ray, intersection, 0, interrupted);

	Color sampledPixelColor;
	int n = scene.raytracer.timeSamples;
	Sampler* sampler = samplers[scene.raytracer.timeSamplerType].get();

	for (int i = 0; i < n; ++i)
	{
		intersection = Intersection();
		ray.time = sampler->getSample(i, n);
		sampledPixelColor += raytrace(scene, ray, intersection, 0, interrupted);
	}

	return sampledPixelColor / (double)(n);
}

Color Raytracer::raytrace(const Scene& scene, const Ray& ray, Intersection& intersection, int iteration, const std::atomic<bool>& interrupted)
{
	Color finalColor = scene.raytracer.backgroundColor;

	if (interrupted)
		return finalColor;

	for (Primitive* primitive : scene.primitives.all)
		primitive->intersect(ray, intersection);

	if (!intersection.wasFound)
		return finalColor;

	Material* material = intersection.primitive->material;
	Color textureColor = material->colorTexture->getColor(intersection.texcoord, intersection.position) * material->colorTexture->intensity;

	if (material->skipLighting)
	{
		finalColor = textureColor;

		if (scene.fog.enabled)
			finalColor = calculateFogColor(scene, intersection, finalColor);

		return finalColor;
	}

	double c1 = -(ray.direction.dot(intersection.normal));
	bool isOutside = (c1 >= 0.0);
	double n1 = isOutside ? 1.0 : material->refractiveIndex;
	double n2 = isOutside ? material->refractiveIndex : 1.0;
	double rf = 1.0;
	double tf = 1.0;

	if (material->normalMapTexture != nullptr)
	{
		TextureNormalType normalType;
		Vector3 textureNormal = material->normalMapTexture->getNormal(intersection.texcoord, intersection.position, normalType);

		if (normalType == TextureNormalType::BUMP)
		{
			// TODO: vector basis should probably use rotated up vector so that bump values go to right directions
			Vector3 N = intersection.normal;
			Vector3 Pu = N.cross(Vector3::UP);
			Vector3 Pv = Pu.cross(N);
			textureNormal *= material->normalMapTexture->intensity;
			Vector3 bumpNormal = N + textureNormal.y * (Pu.cross(N)) + textureNormal.x * (Pv.cross(N));
			intersection.normal = bumpNormal.normalized();
		}
		else if (normalType == TextureNormalType::GRADIENT)
		{
			intersection.normal = (intersection.normal - (textureNormal * material->normalMapTexture->intensity)).normalized();
		}
		else if (normalType == TextureNormalType::NORMAL)
		{
			// TODO: implement normal mapping
		}
	}

	if (material->isFresnel)
	{
		double rf0 = (n2 - n1) / (n2 + n1);
		rf0 = rf0 * rf0;
		rf = rf0 + (1.0 - rf0) * pow(1.0 - std::abs(c1), 5.0);
		tf = 1.0 - rf;
	}

	double transmittance = material->rayTransmittance * tf;
	double reflectance = material->rayReflectance * rf;

	Color refractionColor;
	Color reflectionColor;

	// calculate and trace refracted ray
	if (transmittance > 0.0 && iteration < scene.raytracer.maxRayIterations)
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

			refractedRay.origin = intersection.position + T * scene.raytracer.rayStartOffset;
			refractedRay.direction = T;
			refractedRay.update();

			refractionColor = raytrace(scene, refractedRay, refractedIntersection, iteration + 1, interrupted) * transmittance;

			// only attenuate if ray has traveled inside a primitive
			if (isOutside && refractedIntersection.wasFound && material->enableAttenuation)
			{
				double a = exp(-material->attenuation * refractedIntersection.distance);
				refractionColor = Color::lerp(material->attenuationColor, refractionColor, a);
			}
		}
	}

	// calculate and trace reflected ray
	if (reflectance > 0.0 && iteration < scene.raytracer.maxRayIterations)
	{
		Vector3 R = ray.direction + 2.0 * c1 * intersection.normal;
		R.normalize();

		Ray reflectedRay;
		Intersection reflectedIntersection;

		reflectedRay.origin = intersection.position + R * scene.raytracer.rayStartOffset;
		reflectedRay.direction = R;
		reflectedRay.update();

		reflectionColor = raytrace(scene, reflectedRay, reflectedIntersection, iteration + 1, interrupted) * reflectance;

		// only attenuate if ray has traveled inside a primitive
		if (!isOutside && reflectedIntersection.wasFound && material->enableAttenuation)
		{
			double a = exp(-material->attenuation * reflectedIntersection.distance);
			reflectionColor = Color::lerp(material->attenuationColor, reflectionColor, a);
		}
	}

	// invert normal if facing away
	if (!isOutside)
		intersection.normal = -intersection.normal;

	double ambientOcclusionAmount = 1.0;

	if (scene.lights.ambientLight.enableOcclusion)
		ambientOcclusionAmount = calculateAmbientOcclusionAmount(scene, intersection);

	Color lightColor = calculateLightColor(scene, ray, intersection, ambientOcclusionAmount);
	finalColor = (reflectionColor + refractionColor + lightColor) * textureColor;

	if (scene.fog.enabled && isOutside)
		finalColor = calculateFogColor(scene, intersection, finalColor);

	return finalColor;
}

Color Raytracer::calculateLightColor(const Scene& scene, const Ray& ray, const Intersection& intersection, double ambientOcclusionAmount)
{
	Color lightColor;
	Vector3 directionToCamera = -ray.direction;
	Material* material = intersection.primitive->material;

	lightColor += scene.lights.ambientLight.color * scene.lights.ambientLight.intensity * material->ambientReflectance * ambientOcclusionAmount;

	for (const DirectionalLight& light : scene.lights.directionalLights)
	{
		Color directionalLightColor = calculatePhongShadingColor(intersection.normal, -light.direction, directionToCamera, light, material);
		double shadowAmount = calculateShadowAmount(scene, ray, intersection, light);
		lightColor += directionalLightColor * (1.0 - shadowAmount);
	}

	for (const PointLight& light : scene.lights.pointLights)
	{
		Vector3 directionToLight = (light.position - intersection.position);
		double distanceToLight = directionToLight.length();
		directionToLight.normalize();

		Color pointLightColor = calculatePhongShadingColor(intersection.normal, directionToLight, directionToCamera, light, material);
		double shadowAmount = calculateShadowAmount(scene, ray, intersection, light);
		double distanceAttenuation = std::min(1.0, distanceToLight / light.distance);
		distanceAttenuation = 1.0 - pow(distanceAttenuation, light.attenuation);

		lightColor += pointLightColor * distanceAttenuation * (1.0 - shadowAmount);
	}

	for (const SpotLight& light : scene.lights.spotLights)
	{
		Vector3 directionToLight = (light.position - intersection.position);
		double distanceToLight = directionToLight.length();
		directionToLight.normalize();

		Color spotLightColor = calculatePhongShadingColor(intersection.normal, directionToLight, directionToCamera, light, material);
		double shadowAmount = calculateShadowAmount(scene, ray, intersection, light);
		double distanceAttenuation = std::min(1.0, distanceToLight / light.distance);
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

Color Raytracer::calculatePhongShadingColor(const Vector3& normal, const Vector3& directionToLight, const Vector3& directionToCamera, const Light& light, const Material* material)
{
	Color phongColor;

	double diffuseAmount = directionToLight.dot(normal);

	if (diffuseAmount > 0.0)
	{
		phongColor = light.color * light.intensity * diffuseAmount * material->diffuseReflectance;

		if (!material->specularReflectance.isZero())
		{
			Vector3 reflectionDirection = ((2.0 * diffuseAmount * normal) - directionToLight).normalized();
			double specularAmount = reflectionDirection.dot(directionToCamera);

			if (specularAmount > 0.0)
				phongColor += light.color * light.intensity * pow(specularAmount, material->shininess) * material->specularReflectance;
		}
	}

	return phongColor;
}

Color Raytracer::calculateFogColor(const Scene& scene, const Intersection& intersection, const Color& pixelColor)
{
	double t1 = intersection.distance / scene.fog.distance;
	t1 = std::max(0.0, std::min(t1, 1.0));
	t1 = pow(t1, scene.fog.steepness);

	if (scene.fog.heightDispersion && intersection.position.y > 0.0)
	{
		double t2 = intersection.position.y / scene.fog.height;
		t2 = std::max(0.0, std::min(t2, 1.0));
		t2 = pow(t2, scene.fog.heightSteepness);
		t2 = 1.0 - t2;
		t1 *= t2;
	}

	return Color::lerp(pixelColor, scene.fog.color, t1);
}

double Raytracer::calculateAmbientOcclusionAmount(const Scene& scene, const Intersection& intersection)
{
	double ambientOcclusion = 0.0;
	int permutation = randomDist(generator);
	int n = scene.lights.ambientLight.samples;
	double distribution = scene.lights.ambientLight.distribution;
	Sampler* sampler = samplers[scene.lights.ambientLight.samplerType].get();

	for (int y = 0; y < n; ++y)
	{
		for (int x = 0; x < n; ++x)
		{
			Vector3 sampleDirection = sampler->getHemisphereSample(intersection.onb, distribution, x, y, n, n, permutation);

			Ray sampleRay;
			Intersection sampleIntersection;

			sampleRay.origin = intersection.position + sampleDirection * scene.raytracer.rayStartOffset;
			sampleRay.direction = sampleDirection;
			sampleRay.fastOcclusion = true;
			sampleRay.maxDistance = scene.lights.ambientLight.maxDistance;
			sampleRay.update();

			for (Primitive* primitive : scene.primitives.all)
			{
				if (primitive->intersect(sampleRay, sampleIntersection))
				{
					ambientOcclusion += 1.0;
					break;
				}
			}
		}
	}

	return 1.0 - (ambientOcclusion / (double)(n * n));
}

double Raytracer::calculateShadowAmount(const Scene& scene, const Ray& ray, const Intersection& intersection, const DirectionalLight& light)
{
	Vector3 directionToLight = -light.direction;

	Ray shadowRay;
	Intersection shadowIntersection;

	shadowRay.origin = intersection.position + directionToLight * scene.raytracer.rayStartOffset;
	shadowRay.direction = directionToLight;
	shadowRay.fastOcclusion = true;
	shadowRay.isShadowRay = true;
	shadowRay.maxDistance = std::numeric_limits<double>::max();
	shadowRay.time = ray.time;
	shadowRay.update();

	for (Primitive* primitive : scene.primitives.all)
	{
		if (primitive->intersect(shadowRay, shadowIntersection))
			return 1.0;
	}

	return 0.0;
}

double Raytracer::calculateShadowAmount(const Scene& scene, const Ray& ray, const Intersection& intersection, const PointLight& light)
{
	Vector3 directionToLight = (light.position - intersection.position).normalized();

	if (!light.softShadows)
	{
		Ray shadowRay;
		Intersection shadowIntersection;

		shadowRay.origin = intersection.position + directionToLight * scene.raytracer.rayStartOffset;
		shadowRay.direction = directionToLight;
		shadowRay.fastOcclusion = true;
		shadowRay.isShadowRay = true;
		shadowRay.maxDistance = (light.position - intersection.position).length();
		shadowRay.time = ray.time;
		shadowRay.update();

		for (Primitive* primitive : scene.primitives.all)
		{
			if (primitive->intersect(shadowRay, shadowIntersection))
				return 1.0;
		}

		return 0.0;
	}

	Vector3 lightRight = directionToLight.cross(Vector3::UP).normalized();
	Vector3 lightUp = lightRight.cross(directionToLight).normalized();

	double shadowAmount = 0.0;
	int permutation = randomDist(generator);
	int n = light.samples;
	Sampler* sampler = samplers[light.samplerType].get();

	for (int y = 0; y < n; ++y)
	{
		for (int x = 0; x < n; ++x)
		{
			Vector2 jitter = sampler->getDiskSample(x, y, n, n, permutation) * light.radius;
			Vector3 newLightPosition = light.position + jitter.x * lightRight + jitter.y * lightUp;
			Vector3 newDirectionToLight = (newLightPosition - intersection.position).normalized();

			Ray shadowRay;
			Intersection shadowIntersection;

			shadowRay.origin = intersection.position + newDirectionToLight * scene.raytracer.rayStartOffset;
			shadowRay.direction = newDirectionToLight;
			shadowRay.fastOcclusion = true;
			shadowRay.isShadowRay = true;
			shadowRay.maxDistance = (newLightPosition - intersection.position).length();
			shadowRay.time = ray.time;
			shadowRay.update();

			for (Primitive* primitive : scene.primitives.all)
			{
				if (primitive->intersect(shadowRay, shadowIntersection))
				{
					shadowAmount += 1.0;
					break;
				}
			}
		}
	}

	return shadowAmount / (double)(n * n);
}
