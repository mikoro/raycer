// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <limits>

#include "Raytracing/Raytracer.h"
#include "Raytracing/RaytracerState.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/Material.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

Raytracer::Raytracer()
{
	std::random_device rd;
	gen.seed(rd());
}

void Raytracer::run(RaytracerState& state, std::atomic<bool>& interrupted)
{
	Image& image = *state.image;
	Scene& scene = *state.scene;

	int rayCount = 0;
	int previousRayCount = 0;

	#pragma omp parallel for schedule(dynamic, 1000) reduction(+:rayCount) firstprivate(previousRayCount)
	for (int pixelIndex = 0; pixelIndex < state.pixelCount; ++pixelIndex)
	{
		if (interrupted)
			continue;

		int pixelOffsetIndex = pixelIndex + state.pixelOffset;
		double x = (double)(pixelOffsetIndex % state.sceneWidth);
		double y = (double)(pixelOffsetIndex / state.sceneWidth);
		Vector2 pixel = Vector2(x, y);

		Color pixelColor = shootRays(scene, pixel, rayCount, interrupted);
		image.setPixel(pixelIndex, pixelColor);

		// progress reporting to another thread
		if ((pixelIndex + 1) % 100 == 0)
		{
			state.pixelsProcessed += 100;
			state.raysProcessed += (rayCount - previousRayCount);
			previousRayCount = rayCount;
		}
	}

	state.raysProcessed = rayCount;

	if (!interrupted)
		state.pixelsProcessed = state.pixelCount;
}

Color Raytracer::shootRays(const Scene& scene, const Vector2& pixel, int& rayCount, const std::atomic<bool>& interrupted)
{
	if (scene.multisampler.type == MultisampleType::NONE)
	{
		Vector2 sampledPixel = sampler.getCentroidSample(pixel);
		Ray ray = scene.camera.getRay(sampledPixel);
		traceRay(scene, ray, rayCount, interrupted);

		return ray.color;
	}

	Color pixelColor;
	int pixelCount = 0;
	int permutation = intDist(gen);
	int n = scene.multisampler.multisamples;

	for (int y = 0; y < n; ++y)
	{
		for (int x = 0; x < n; ++x)
		{
			Vector2 sampledPixel;

			if (scene.multisampler.type == MultisampleType::RANDOM)
				sampledPixel = sampler.getRandomSample(pixel);
			else if (scene.multisampler.type == MultisampleType::REGULAR_GRID)
				sampledPixel = sampler.getRegularGridSample(pixel, x, y, n, n);
			else if (scene.multisampler.type == MultisampleType::JITTER)
				sampledPixel = sampler.getJitteredSample(pixel, x, y, n, n);
			else if (scene.multisampler.type == MultisampleType::CORRELATED_MULTI_JITTER)
				sampledPixel = sampler.getCmjSample(pixel, x, y, n, n, permutation);

			Ray ray = scene.camera.getRay(sampledPixel);
			traceRay(scene, ray, rayCount, interrupted);

			if (ray.intersection.wasFound)
			{
				pixelColor += ray.color;
				++pixelCount;
			}
		}
	}

	return pixelColor / (double)pixelCount;
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

void Raytracer::traceRay(const Scene& scene, Ray& ray, int& rayCount, const std::atomic<bool>& interrupted)
{
	if (interrupted)
		return;

	++rayCount;

	for (const Primitive* primitive : scene.primitivesList)
		primitive->intersect(ray);

	if (!ray.intersection.wasFound)
		return;

	Material* material = scene.materialsMap.at(ray.intersection.materialId);
	Texture* texture = scene.texturesMap.at(material->textureId);

	Color textureColor = texture->getColor(ray.intersection.position, ray.intersection.texcoord) * texture->intensity;
	Color reflectionColor;
	Color refractionColor;

	Vector3 D = ray.direction;
	Vector3 N = ray.intersection.normal;
	Vector3 P = ray.intersection.position;

	double c1 = -(D.dot(N));
	bool isOutside = (c1 >= 0.0);
	double n1 = isOutside ? 1.0 : material->refractiveIndex;
	double n2 = isOutside ? material->refractiveIndex : 1.0;
	double rf = 1.0;
	double tf = 1.0;

	if (material->skipLighting)
	{
		ray.color = textureColor;

		if (scene.fog.enabled)
			ray.color = calculateFogColor(scene, ray);

		return;
	}

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

	// calculate and trace refracted ray
	if (transmittance > 0.0 && ray.iterations < scene.tracer.maxIterations)
	{
		double n = n1 / n2;
		double c2 = 1.0 - (n * n) * (1.0 - c1 * c1);

		// no total internal reflection
		if (c2 > 0.0)
		{
			Vector3 T = D * n + (c1 * n - sqrt(c2)) * N;
			T.normalize();

			Ray refractedRay;
			refractedRay.origin = P + T * scene.tracer.rayStartOffset;
			refractedRay.direction = T;
			refractedRay.iterations = ray.iterations + 1;

			traceRay(scene, refractedRay, rayCount, interrupted);

			if (ray.intersection.wasFound)
			{
				refractionColor = refractedRay.color * transmittance;

				if (isOutside && material->attenuate)
				{
					double a = exp(-material->attenuation * refractedRay.intersection.distance);
					refractionColor = Color::lerp(material->attenuationColor, refractionColor, a);
				}
			}
		}
	}

	// calculate and trace reflected ray
	if (reflectance > 0.0 && ray.iterations < scene.tracer.maxIterations)
	{
		Vector3 R = D + 2.0 * c1 * N;
		R.normalize();

		Ray reflectedRay;
		reflectedRay.origin = P + R * scene.tracer.rayStartOffset;
		reflectedRay.direction = R;
		reflectedRay.iterations = ray.iterations + 1;

		traceRay(scene, reflectedRay, rayCount, interrupted);

		if (ray.intersection.wasFound)
		{
			reflectionColor = reflectedRay.color * reflectance;

			if (!isOutside && material->attenuate)
			{
				double a = exp(-material->attenuation * reflectedRay.intersection.distance);
				reflectionColor = Color::lerp(material->attenuationColor, reflectionColor, a);
			}
		}
	}

	Color lightColor;

	if (isOutside)
		lightColor = calculateLightColor(scene, ray);

	ray.color = (reflectionColor + refractionColor + lightColor) * textureColor;

	if (scene.fog.enabled && isOutside)
		ray.color = calculateFogColor(scene, ray);
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

namespace
{
	bool isInShadow(const Scene& scene, const Vector3& P, const Vector3& L, double distanceToLight)
	{
		Ray shadowRay;
		shadowRay.origin = P + L * scene.tracer.rayStartOffset;
		shadowRay.direction = L;

		for (const Primitive* primitive : scene.primitivesList)
		{
			if (!primitive->nonShadowing)
				primitive->intersect(shadowRay);
		}

		return shadowRay.intersection.wasFound && shadowRay.intersection.distance < distanceToLight;
	}

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
}

Color Raytracer::calculateLightColor(const Scene& scene, const Ray& ray)
{
	Material* material = scene.materialsMap.at(ray.intersection.materialId);

	Color lightColor;

	Vector3 P = ray.intersection.position;
	Vector3 N = ray.intersection.normal;
	Vector3 V = -ray.direction;


	for (const AmbientLight& ambientLight : scene.lights.ambientLights)
		lightColor += ambientLight.color * ambientLight.intensity * material->ambientness;

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

Color Raytracer::calculateFogColor(const Scene& scene, const Ray& ray)
{
	double t1 = ray.intersection.distance / scene.fog.distance;
	t1 = std::max(0.0, std::min(t1, 1.0));
	t1 = pow(t1, scene.fog.steepness);

	if (scene.fog.heightDispersion && ray.intersection.position.y > 0.0)
	{
		double t2 = ray.intersection.position.y / scene.fog.height;
		t2 = std::max(0.0, std::min(t2, 1.0));
		t2 = pow(t2, scene.fog.heightSteepness);
		t2 = 1.0 - t2;
		t1 *= t2;
	}

	return Color::lerp(ray.color, scene.fog.color, t1);
}
