// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>

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

using namespace Raycer;

Color doPhongShading(const Vector3& N, const Vector3& L, const Vector3& V, const Light* light, const Material* material);
bool isInShadow(const Scene& scene, const Vector3& P, const Vector3& L, double distanceToLight);

Raytracer::Raytracer()
{
	std::random_device rd;
	gen.seed(rd());
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

		Color pixelColor = generatePixelSamples(scene, pixelCoordinate, interrupted);
		image.setPixel(pixelIndex, pixelColor);

		// progress reporting to another thread
		if ((pixelIndex + 1) % 100 == 0)
			state.pixelsProcessed += 100;
	}

	if (!interrupted)
		state.pixelsProcessed = state.pixelCount;
}

Color Raytracer::generatePixelSamples(const Scene& scene, const Vector2& pixelCoordinate, const std::atomic<bool>& interrupted)
{
	if (scene.multisampler.type == MultisampleType::NONE)
	{
		Vector2 sampledPixelCoordinate = pixelCoordinate + Vector2(0.5, 0.5);
		return generateCameraSamples(scene, sampledPixelCoordinate, interrupted);
	}

	Color sampledPixelColor;
	int permutation = intDist(gen);
	int n = scene.multisampler.multisamples;

	for (int y = 0; y < n; ++y)
	{
		for (int x = 0; x < n; ++x)
		{
			Vector2 sampledPixelCoordinate;

			if (scene.multisampler.type == MultisampleType::CORRELATED_MULTI_JITTER)
				sampledPixelCoordinate = pixelCoordinate + sampler.getCmjSample(x, y, n, n, permutation);
			else if (scene.multisampler.type == MultisampleType::RANDOM)
				sampledPixelCoordinate = pixelCoordinate + sampler.getRandomSample();
			else if (scene.multisampler.type == MultisampleType::REGULAR_GRID)
				sampledPixelCoordinate = pixelCoordinate + sampler.getRegularGridSample(x, y, n, n);
			else if (scene.multisampler.type == MultisampleType::JITTER)
				sampledPixelCoordinate = pixelCoordinate + sampler.getJitteredSample(x, y, n, n);

			sampledPixelColor += generateCameraSamples(scene, sampledPixelCoordinate, interrupted);
		}
	}

	return sampledPixelColor / (double)(n * n);
}

Color Raytracer::generateCameraSamples(const Scene& scene, const Vector2& sampledPixelCoordinate, const std::atomic<bool>& interrupted)
{
	Ray primaryRay = scene.camera.getRay(sampledPixelCoordinate);
	Intersection primaryIntersection;

	if (!scene.camera.depthOfField)
		return raytrace(scene, primaryRay, primaryIntersection, 0, interrupted);

	Color sampledPixelColor;
	int permutation = intDist(gen);
	int n = scene.camera.samples;
	double apertureSize = scene.camera.apertureSize;
	double focalLength = scene.camera.focalLenght;

	Vector3 cameraOrigin = scene.camera.position;
	Vector3 cameraRight = scene.camera.right;
	Vector3 cameraUp = scene.camera.up;
	Vector3 focalPoint = primaryRay.origin + primaryRay.direction * focalLength;

	for (int y = 0; y < n; ++y)
	{
		for (int x = 0; x < n; ++x)
		{
			Ray sampleRay;
			Intersection sampleIntersection;

			Vector2 diskCoordinate = sampler.getCmjDiskSample(x, y, n, n, permutation);
			sampleRay.origin = cameraOrigin + ((diskCoordinate.x * apertureSize) * cameraRight + (diskCoordinate.y * apertureSize) * cameraUp);
			sampleRay.direction = (focalPoint - sampleRay.origin).normalized();
			
			sampledPixelColor += raytrace(scene, sampleRay, sampleIntersection, 0, interrupted);
		}
	}

	return sampledPixelColor / (double)(n * n);
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
	Color finalColor;

	if (interrupted)
		return finalColor;

	for (const Primitive* primitive : scene.primitivesList)
		primitive->intersect(ray, intersection);

	if (!intersection.wasFound)
		return finalColor;

	Material* material = scene.materialsMap.at(intersection.materialId);
	Texture* texture = scene.texturesMap.at(material->textureId);

	Color textureColor = texture->getColor(intersection.position, intersection.texcoord) * texture->intensity;

	if (material->skipLighting)
	{
		finalColor = textureColor;

		if (scene.fog.enabled)
			finalColor = calculateFogColor(scene, intersection, finalColor);

		return finalColor;
	}

	Vector3 D = ray.direction;
	Vector3 N = intersection.normal;
	Vector3 P = intersection.position;

	double c1 = -(D.dot(N));
	bool isOutside = (c1 >= 0.0);
	double n1 = isOutside ? 1.0 : material->refractiveIndex;
	double n2 = isOutside ? material->refractiveIndex : 1.0;
	double rf = 1.0;
	double tf = 1.0;

	if (material->fresnel)
	{
		double rf0 = (n2 - n1) / (n2 + n1);
		rf0 = rf0 * rf0;
		rf = rf0 + (1.0 - rf0) * pow(1.0 - fabs(c1), 5.0);
		tf = 1.0 - rf;
	}

	if (!isOutside)
		N = -N;

	double transmittance = material->transmittance * tf;
	double reflectance = material->reflectance * rf;

	Color refractionColor;
	Color reflectionColor;

	// calculate and trace refracted ray
	if (transmittance > 0.0 && iteration < scene.tracer.maxIterations)
	{
		double n = n1 / n2;
		double c2 = 1.0 - (n * n) * (1.0 - c1 * c1);

		// no total internal reflection
		if (c2 > 0.0)
		{
			Vector3 T = D * n + (c1 * n - sqrt(c2)) * N;
			T.normalize();

			Ray refractedRay;
			Intersection refractedIntersection;

			refractedRay.origin = P + T * scene.tracer.rayStartOffset;
			refractedRay.direction = T;

			Color color = raytrace(scene, refractedRay, refractedIntersection, iteration + 1, interrupted);

			if (refractedIntersection.wasFound)
			{
				refractionColor = color * transmittance;

				if (isOutside && material->attenuate)
				{
					double a = exp(-material->attenuation * refractedIntersection.distance);
					refractionColor = Color::lerp(material->attenuationColor, refractionColor, a);
				}
			}
		}
	}

	// calculate and trace reflected ray
	if (reflectance > 0.0 && iteration < scene.tracer.maxIterations)
	{
		Vector3 R = D + 2.0 * c1 * N;
		R.normalize();

		Ray reflectedRay;
		Intersection reflectedIntersection;

		reflectedRay.origin = P + R * scene.tracer.rayStartOffset;
		reflectedRay.direction = R;

		Color color = raytrace(scene, reflectedRay, reflectedIntersection, iteration + 1, interrupted);

		if (reflectedIntersection.wasFound)
		{
			reflectionColor = color * reflectance;

			if (!isOutside && material->attenuate)
			{
				double a = exp(-material->attenuation * reflectedIntersection.distance);
				reflectionColor = Color::lerp(material->attenuationColor, reflectionColor, a);
			}
		}
	}

	double ambientOcclusion = 1.0;

	if (scene.lights.ambientLight.ambientOcclusion && isOutside)
		ambientOcclusion = calculateAmbientOcclusion(scene, intersection);

	Color lightColor;

	if (isOutside)
		lightColor = calculateLightColor(scene, ray, intersection, ambientOcclusion);

	finalColor = (reflectionColor + refractionColor + lightColor) * textureColor;

	if (scene.fog.enabled && isOutside)
		finalColor = calculateFogColor(scene, intersection, finalColor);

	return finalColor;
}

double Raytracer::calculateAmbientOcclusion(const Scene& scene, const Intersection& intersection)
{
	Vector3 origin = intersection.position;
	Vector3 up = Vector3(0.001, 1.0, 0.001);
	Vector3 w = intersection.normal;
	Vector3 v = w.cross(up).normalized();
	Vector3 u = v.cross(w).normalized();

	double ambientOcclusion = 0.0;
	int permutation = intDist(gen);
	int n = scene.lights.ambientLight.samples;
	double distribution = scene.lights.ambientLight.distribution;
	double distance = scene.lights.ambientLight.distance;

	for (int y = 0; y < n; ++y)
	{
		for (int x = 0; x < n; ++x)
		{
			Vector3 sampleDirection = sampler.getCmjHemisphereSample(u, v, w, distribution, x, y, n, n, permutation);

			Ray sampleRay;
			Intersection sampleIntersection;

			sampleRay.origin = origin + sampleDirection * scene.tracer.rayStartOffset;
			sampleRay.direction = sampleDirection;

			for (const Primitive* primitive : scene.primitivesList)
				primitive->intersect(sampleRay, sampleIntersection);

			if (sampleIntersection.wasFound)
				ambientOcclusion += (1.0 - std::min(sampleIntersection.distance / distance, 1.0));
		}
	}

	return 1.0 - (ambientOcclusion / (double)(n * n));
}

Color Raytracer::calculateLightColor(const Scene& scene, const Ray& viewRay, const Intersection& intersection, double ambientOcclusion)
{
	Material* material = scene.materialsMap.at(intersection.materialId);

	Color lightColor;

	Vector3 P = intersection.position;
	Vector3 N = intersection.normal;
	Vector3 V = -viewRay.direction;

	lightColor += scene.lights.ambientLight.color * scene.lights.ambientLight.intensity * material->ambientness * ambientOcclusion;

	for (const DirectionalLight& light : scene.lights.directionalLights)
	{
		Vector3 L = -light.direction;

		if (isInShadow(scene, P, L, std::numeric_limits<double>::max()))
			continue;

		lightColor += doPhongShading(N, L, V, &light, material);
	}

	for (const PointLight& light : scene.lights.pointLights)
	{
		Vector3 L = (light.position - P);
		double distance = L.length();
		L.normalize();

		if (isInShadow(scene, P, L, distance))
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

		if (isInShadow(scene, P, L, distance))
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
		phongColor = light->color * light->intensity * d1 * material->diffuseness;

		if (material->specularity > 0.0)
		{
			// reflected light direction
			Vector3 R = (2.0 * d1 * N) - L;
			R.normalize();

			// specular amount
			double d2 = R.dot(V);

			if (d2 > 0.0)
				phongColor += light->color * light->intensity * pow(d2, material->shininess) * material->specularity;
		}
	}

	return phongColor;
}

bool isInShadow(const Scene& scene, const Vector3& P, const Vector3& L, double distanceToLight)
{
	Ray shadowRay;
	Intersection shadowIntersection;

	shadowRay.origin = P + L * scene.tracer.rayStartOffset;
	shadowRay.direction = L;

	for (const Primitive* primitive : scene.primitivesList)
	{
		if (!primitive->nonShadowing)
			primitive->intersect(shadowRay, shadowIntersection);
	}

	return shadowIntersection.wasFound && shadowIntersection.distance < distanceToLight;
}
