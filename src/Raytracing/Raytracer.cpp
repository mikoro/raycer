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

using namespace Raycer;

Color doPhongShading(const Vector3& N, const Vector3& L, const Vector3& V, const Light* light, const Material* material);
bool isInShadow(const Scene& scene, const Ray& ray, const Vector3& P, const Vector3& L, double distanceToLight);

Raytracer::Raytracer()
{
	std::random_device rd;
	generator.seed(rd());
}

void Raytracer::initialize(const Scene& scene)
{
	switch (scene.raytracing.multiSamplerType)
	{
		case SamplerType::RANDOM: multiSampler = new RandomSampler(); break;
		case SamplerType::REGULAR: multiSampler = new RegularSampler(); break;
		case SamplerType::JITTERED: multiSampler = new JitteredSampler(); break;
		case SamplerType::CMJ: multiSampler = new CMJSampler(); break;
		default: throw std::runtime_error("Invalid multi sampler type");
	}

	switch (scene.raytracing.dofSamplerType)
	{
		case SamplerType::RANDOM: dofSampler = new RandomSampler(); break;
		case SamplerType::REGULAR: dofSampler = new RegularSampler(); break;
		case SamplerType::JITTERED: dofSampler = new JitteredSampler(); break;
		case SamplerType::CMJ: dofSampler = new CMJSampler(); break;
		default: throw std::runtime_error("Invalid dof sampler type");
	}

	switch (scene.raytracing.timeSamplerType)
	{
		case SamplerType::RANDOM: timeSampler = new RandomSampler(); break;
		case SamplerType::REGULAR: timeSampler = new RegularSampler(); break;
		case SamplerType::JITTERED: timeSampler = new JitteredSampler(); break;
		case SamplerType::CMJ: timeSampler = new CMJSampler(); break;
		default: throw std::runtime_error("Invalid time sampler type");
	}

	switch (scene.lights.ambientLight.samplerType)
	{
		case SamplerType::RANDOM: ambientOcclusionSampler = new RandomSampler(); break;
		case SamplerType::REGULAR: ambientOcclusionSampler = new RegularSampler(); break;
		case SamplerType::JITTERED: ambientOcclusionSampler = new JitteredSampler(); break;
		case SamplerType::CMJ: ambientOcclusionSampler = new CMJSampler(); break;
		default: throw std::runtime_error("Invalid ambient occlusion sampler type");
	}
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
}

Color Raytracer::generateMultiSamples(const Scene& scene, const Vector2& pixelCoordinate, const std::atomic<bool>& interrupted)
{
	if (scene.raytracing.multiSamples == 0)
	{
		Vector2 sampledPixelCoordinate = pixelCoordinate + Vector2(0.5, 0.5);
		return generateDofSamples(scene, sampledPixelCoordinate, interrupted);
	}

	Color sampledPixelColor;
	int permutation = randomDist(generator);
	int n = scene.raytracing.multiSamples;

	for (int y = 0; y < n; ++y)
	{
		for (int x = 0; x < n; ++x)
		{
			Vector2 sampledPixelCoordinate = pixelCoordinate + multiSampler->getSample2D(x, y, n, n, permutation);
			sampledPixelColor += generateDofSamples(scene, sampledPixelCoordinate, interrupted);
		}
	}

	return sampledPixelColor / (double)(n * n);
}

Color Raytracer::generateDofSamples(const Scene& scene, const Vector2& pixelCoordinate, const std::atomic<bool>& interrupted)
{
	Ray ray = scene.camera.getRay(pixelCoordinate);

	if (ray.isInvalid && scene.raytracing.dofSamples == 0)
		return Color::BLACK;

	if (scene.raytracing.dofSamples == 0)
		return generateTimeSamples(scene, ray, interrupted);

	Color sampledPixelColor;
	int permutation = randomDist(generator);
	int n = scene.raytracing.dofSamples;
	double apertureSize = scene.camera.apertureSize;
	double focalLength = scene.camera.focalLenght;

	Vector3 cameraOrigin = scene.camera.position;
	Vector3 cameraRight = scene.camera.right;
	Vector3 cameraUp = scene.camera.up;

	for (int y = 0; y < n; ++y)
	{
		for (int x = 0; x < n; ++x)
		{
			Vector2 jitter = dofSampler->getSample2D(x, y, n, n, permutation) - Vector2(0.5, 0.5);
			Ray primaryRay = scene.camera.getRay(pixelCoordinate + jitter);

			if (primaryRay.isInvalid)
				continue;

			Vector3 focalPoint = primaryRay.origin + primaryRay.direction * focalLength;
			Vector2 diskCoordinate = dofSampler->getSampleDisk(x, y, n, n, permutation);

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

	if (scene.raytracing.timeSamples == 0)
		return raytrace(scene, ray, intersection, 0, interrupted);

	Color sampledPixelColor;
	int n = scene.raytracing.timeSamples;

	for (int i = 0; i < n; ++i)
	{
		intersection = Intersection();
		ray.time = timeSampler->getSample1D(i, n);
		sampledPixelColor += raytrace(scene, ray, intersection, 0, interrupted);
	}

	return sampledPixelColor / (double)(n);
}

/*

D = ray direction (unit)
N = intersection surface normal (unit)
R = reflected ray direction (unit)
T = transmitted (refracted) ray direction (unit)
P = intersection position
n1 = from refractive index
n2 = to refractive index
n = relative refractive index
c1 = ray-surface cosine, > 0: ray comes from outside,  < 0: ray comes from inside
c2 = < 0: total internal reflection, no transmitted ray
rf = fresnel reflectance factor
tf = fresnel transmittance factor
rf0 = fresnel reflectance at normal incidence
d = distance traveled inside

*/

Color Raytracer::raytrace(const Scene& scene, const Ray& ray, Intersection& intersection, int iteration, const std::atomic<bool>& interrupted)
{
	Color finalColor = scene.raytracing.backgroundColor;

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
		rf = rf0 + (1.0 - rf0) * pow(1.0 - fabs(c1), 5.0);
		tf = 1.0 - rf;
	}

	double transmittance = material->rayTransmittance * tf;
	double reflectance = material->rayReflectance * rf;

	Color refractionColor;
	Color reflectionColor;

	// calculate and trace refracted ray
	if (transmittance > 0.0 && iteration < scene.raytracing.maxIterations)
	{
		double n = n1 / n2;
		double c2 = 1.0 - (n * n) * (1.0 - c1 * c1);

		// no total internal reflection
		if (c2 > 0.0)
		{
			Vector3 T = ray.direction * n + (fabs(c1) * n - sqrt(c2)) * intersection.normal;
			T.normalize();

			Ray refractedRay;
			Intersection refractedIntersection;

			refractedRay.origin = intersection.position + T * scene.raytracing.rayStartOffset;
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
	if (reflectance > 0.0 && iteration < scene.raytracing.maxIterations)
	{
		Vector3 R = ray.direction + 2.0 * c1 * intersection.normal;
		R.normalize();

		Ray reflectedRay;
		Intersection reflectedIntersection;

		reflectedRay.origin = intersection.position + R * scene.raytracing.rayStartOffset;
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

	double ambientOcclusion = 1.0;

	if (scene.lights.ambientLight.enableOcclusion)
		ambientOcclusion = calculateAmbientOcclusion(scene, intersection);

	Color lightColor = calculateLightColor(scene, ray, intersection, ambientOcclusion);
	finalColor = (reflectionColor + refractionColor + lightColor) * textureColor;

	if (scene.fog.enabled && isOutside)
		finalColor = calculateFogColor(scene, intersection, finalColor);

	return finalColor;
}

double Raytracer::calculateAmbientOcclusion(const Scene& scene, const Intersection& intersection)
{
	double ambientOcclusion = 0.0;
	int permutation = randomDist(generator);
	int n = scene.lights.ambientLight.samples;
	double distribution = scene.lights.ambientLight.distribution;

	for (int y = 0; y < n; ++y)
	{
		for (int x = 0; x < n; ++x)
		{
			Vector3 sampleDirection = ambientOcclusionSampler->getSampleHemisphere(intersection.onb, distribution, x, y, n, n, permutation);

			Ray sampleRay;
			Intersection sampleIntersection;

			sampleRay.origin = intersection.position + sampleDirection * scene.raytracing.rayStartOffset;
			sampleRay.direction = sampleDirection;
			sampleRay.fastOcclusion = true;
			sampleRay.tmax = scene.lights.ambientLight.maxDistance;
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

Color Raytracer::calculateLightColor(const Scene& scene, const Ray& ray, const Intersection& intersection, double ambientOcclusion)
{
	Material* material = intersection.primitive->material;

	Color lightColor;

	Vector3 P = intersection.position;
	Vector3 N = intersection.normal;
	Vector3 V = -ray.direction;

	lightColor += scene.lights.ambientLight.color * scene.lights.ambientLight.intensity * material->ambientReflectance * ambientOcclusion;

	for (const DirectionalLight& light : scene.lights.directionalLights)
	{
		Vector3 L = -light.direction;

		if (isInShadow(scene, ray, P, L, std::numeric_limits<double>::max()))
			continue;

		lightColor += doPhongShading(N, L, V, &light, material);
	}

	for (const PointLight& light : scene.lights.pointLights)
	{
		Vector3 L = (light.position - P);
		double distance = L.length();
		L.normalize();

		if (isInShadow(scene, ray, P, L, distance))
			continue;

		Color pointLightColor = doPhongShading(N, L, V, &light, material);
		double attenuation = std::min(1.0, distance / light.distance);
		attenuation = 1.0 - pow(attenuation, light.attenuation);

		lightColor += pointLightColor * attenuation;
	}

	for (const SpotLight& light : scene.lights.spotLights)
	{
		Vector3 L = (light.position - P);
		double distance = L.length();
		L.normalize();

		if (isInShadow(scene, ray, P, L, distance))
			continue;

		Color spotLightColor = doPhongShading(N, L, V, &light, material);

		double distanceAttenuation = std::min(1.0, distance / light.distance);
		distanceAttenuation = 1.0 - pow(distanceAttenuation, light.distanceAttenuation);
		double sideAttenuation = light.direction.dot(-L);

		if (sideAttenuation > 0.0)
		{
			sideAttenuation = std::min(1.0, (1.0 - sideAttenuation) / (light.angle / 180.0));
			sideAttenuation = 1.0 - pow(sideAttenuation, light.sideAttenuation);
			lightColor += spotLightColor * distanceAttenuation * sideAttenuation;
		}
	}

	return lightColor;
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

/*

P = intersection position
N = intersection surface normal (unit)
V = direction to viewer (unit)
L = direction to light (unit)
R = reflected light direction (unit)
d1 = diffuse light relative amount, only when > 0
d2 = specular light relative amount, only when > 0 and d1 > 0

*/

Color doPhongShading(const Vector3& N, const Vector3& L, const Vector3& V, const Light* light, const Material* material)
{
	Color phongColor;

	// diffuse amount
	double d1 = L.dot(N);

	if (d1 > 0.0)
	{
		phongColor = light->color * light->intensity * d1 * material->diffuseReflectance;

		if (!material->specularReflectance.isZero())
		{
			// reflected light direction
			Vector3 R = (2.0 * d1 * N) - L;
			R.normalize();

			// specular amount
			double d2 = R.dot(V);

			if (d2 > 0.0)
				phongColor += light->color * light->intensity * pow(d2, material->shininess) * material->specularReflectance;
		}
	}

	return phongColor;
}

bool isInShadow(const Scene& scene, const Ray& ray, const Vector3& P, const Vector3& L, double distanceToLight)
{
	Ray shadowRay;
	Intersection shadowIntersection;

	shadowRay.origin = P + L * scene.raytracing.rayStartOffset;
	shadowRay.direction = L;
	shadowRay.fastOcclusion = true;
	shadowRay.isShadowRay = true;
	shadowRay.tmax = distanceToLight;
	shadowRay.time = ray.time;
	shadowRay.update();

	for (Primitive* primitive : scene.primitives.all)
	{
		if (primitive->intersect(shadowRay, shadowIntersection))
			return true;
	}

	return false;
}
