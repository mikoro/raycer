// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#ifdef _MSC_VER
#define ALIGN(x) __declspec(align(x))
#elif __GNUC__
#define ALIGN(x) __attribute__ ((aligned(x)))
#endif

namespace Raycer
{
	namespace OpenCL
	{
		struct State
		{
			cl_float ALIGN(4) time;
			cl_int ALIGN(4) directionalLightCount;
			cl_int ALIGN(4) pointLightCount;
			cl_int ALIGN(4) spotLightCount;
			cl_int ALIGN(4) planeCount;
			cl_int ALIGN(4) sphereCount;
			cl_int ALIGN(4) boxCount;
			cl_int ALIGN(4) triangleCount;
		};

		struct Camera
		{
			cl_float4 ALIGN(16) position;
			cl_float4 ALIGN(16) forward;
			cl_float4 ALIGN(16) right;
			cl_float4 ALIGN(16) up;
			cl_float4 ALIGN(16) imagePlaneCenter;
			cl_int ALIGN(4) projectionType;
			cl_float ALIGN(4) fov;
			cl_float ALIGN(4) orthoSize;
			cl_float ALIGN(4) fishEyeAngle;
			cl_float ALIGN(4) apertureSize;
			cl_float ALIGN(4) focalLenght;
			cl_float ALIGN(4) imagePlaneWidth;
			cl_float ALIGN(4) imagePlaneHeight;
			cl_float ALIGN(4) aspectRatio;
		};

		struct Raytracer
		{
			cl_float4 ALIGN(16) backgroundColor;
			cl_float4 ALIGN(16) offLensColor;
			cl_int ALIGN(4) maxRayIterations;
			cl_float ALIGN(4) rayStartOffset;
			cl_int ALIGN(4) multiSamples;
			cl_int ALIGN(4) dofSamples;
			cl_int ALIGN(4) timeSamples;
		};

		struct ToneMapper
		{
			cl_int ALIGN(4) type;
			cl_int ALIGN(4) applyGamma;
			cl_int ALIGN(4) shouldClamp;
			cl_float ALIGN(4) gamma;
			cl_float ALIGN(4) exposure;
			cl_float ALIGN(4) key;
			cl_float ALIGN(4) maxLuminance;
		};

		struct SimpleFog
		{
			cl_float4 ALIGN(16) color;
			cl_int ALIGN(4) enabled;
			cl_float ALIGN(4) distance;
			cl_float ALIGN(4) steepness;
			cl_int ALIGN(4) heightDispersion;
			cl_float ALIGN(4) height;
			cl_float ALIGN(4) heightSteepness;
		};

		struct Material
		{
			cl_float4 ALIGN(16) ambientReflectance;
			cl_float4 ALIGN(16) diffuseReflectance;
			cl_float4 ALIGN(16) specularReflectance;
			cl_float4 ALIGN(16) attenuationColor;
			cl_float2 ALIGN(8) texcoordScale;
			cl_int ALIGN(4) id;
			cl_float ALIGN(4) shininess;
			cl_int ALIGN(4) skipLighting;
			cl_int ALIGN(4) nonShadowing;
			cl_float ALIGN(4) rayReflectance;
			cl_float ALIGN(4) rayTransmittance;
			cl_float ALIGN(4) refractiveIndex;
			cl_int ALIGN(4) isFresnel;
			cl_int ALIGN(4) enableAttenuation;
			cl_float ALIGN(4) attenuation;
		};

		struct AmbientLight
		{
			cl_float4 ALIGN(16) color;
			cl_float ALIGN(4) intensity;
			cl_int ALIGN(4) enableOcclusion;
			cl_float ALIGN(4) maxDistance;
			cl_int ALIGN(4) samplerType;
			cl_int ALIGN(4) samples;
			cl_float ALIGN(4) distribution;
		};

		struct DirectionalLight
		{
			cl_float4 ALIGN(16) color;
			cl_float4 ALIGN(16) direction;
			cl_float ALIGN(4) intensity;
		};

		struct PointLight
		{
			cl_float4 ALIGN(16) color;
			cl_float4 ALIGN(16) position;
			cl_float ALIGN(4) intensity;
			cl_float ALIGN(4) distance;
			cl_float ALIGN(4) attenuation;
			cl_int ALIGN(4) softShadows;
			cl_float ALIGN(4) radius;
			cl_int ALIGN(4) samplerType;
			cl_int ALIGN(4) samples;
		};

		struct SpotLight
		{
			cl_float4 ALIGN(16) color;
			cl_float4 ALIGN(16) position;
			cl_float4 ALIGN(16) direction;
			cl_float ALIGN(4) intensity;
			cl_float ALIGN(4) distance;
			cl_float ALIGN(4) attenuation;
			cl_float ALIGN(4) sideAttenuation;
			cl_float ALIGN(4) angle;
			cl_int ALIGN(4) softShadows;
			cl_float ALIGN(4) radius;
			cl_int ALIGN(4) samplerType;
			cl_int ALIGN(4) samples;
		};

		struct Plane
		{
			cl_float4 ALIGN(16) position;
			cl_float4 ALIGN(16) normal;
			cl_int ALIGN(4) materialIndex;
		};

		struct Sphere
		{
			cl_float4 ALIGN(16) position;
			cl_float4 ALIGN(16) displacement;
			cl_float ALIGN(4) radius;
			cl_int ALIGN(4) materialIndex;
		};

		struct Box
		{
			cl_float4 ALIGN(16) position;
			cl_float4 ALIGN(16) extent;
			cl_int ALIGN(4) materialIndex;
		};

		struct Triangle
		{
			cl_float4 ALIGN(16) vertices[3];
			cl_float4 ALIGN(16) normals[3];
			cl_float4 ALIGN(16) texcoords[3];
			cl_float4 ALIGN(16) normal;
			cl_float4 ALIGN(16) tangent;
			cl_float4 ALIGN(16) bitangent;
			cl_int ALIGN(4) materialIndex;
		};
	}
}
