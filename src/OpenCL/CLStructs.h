// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#ifdef ALIGN
#undef ALIGN
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
			cl_int ALIGN(4) triangleCount;
			cl_int ALIGN(4) bvhNodeCount;
		};

		struct General
		{
			cl_float4 ALIGN(16) backgroundColor;
			cl_float4 ALIGN(16) offLensColor;
			cl_float ALIGN(4) rayStartOffset;
			cl_int ALIGN(4) maxRayIterations;
			cl_int ALIGN(4) maxPathLength;
			cl_int ALIGN(4) pathSampleCount;
			cl_int ALIGN(4) multiSampleCountSqrt;
			cl_int ALIGN(4) timeSampleCount;
			cl_int ALIGN(4) cameraSampleCountSqrt;
			cl_int ALIGN(4) visualizeDepth;
			cl_float ALIGN(4) visualizeDepthMaxDistance;
			cl_int ALIGN(4) enableNormalMapping;
		};

		struct Camera
		{
			cl_float4 ALIGN(16) position;
			cl_float4 ALIGN(16) forward;
			cl_float4 ALIGN(16) right;
			cl_float4 ALIGN(16) up;
			cl_float4 ALIGN(16) imagePlaneCenter;
			cl_float4 ALIGN(16) translateInTime;
			cl_float4 ALIGN(16) rotateInTime;
			cl_int ALIGN(4) projectionType;
			cl_int ALIGN(4) isTimeVariant;
			cl_int ALIGN(4) hasMoved;
			cl_float ALIGN(4) fov;
			cl_float ALIGN(4) orthoSize;
			cl_float ALIGN(4) fishEyeAngle;
			cl_float ALIGN(4) apertureSize;
			cl_float ALIGN(4) focalDistance;
			cl_float ALIGN(4) aspectRatio;
			cl_float ALIGN(4) imagePlaneDistance;
			cl_float ALIGN(4) imagePlaneWidth;
			cl_float ALIGN(4) imagePlaneHeight;
		};

		struct ToneMapper
		{
			cl_int ALIGN(4) type;
			cl_int ALIGN(4) applyGamma;
			cl_int ALIGN(4) shouldClamp;
			cl_float ALIGN(4) gamma;
			cl_float ALIGN(4) exposure;
			cl_float ALIGN(4) key;
			cl_int ALIGN(4) enableAveraging;
			cl_float ALIGN(4) averagingAlpha;
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
			cl_float4 ALIGN(16) emittance;
			cl_float4 ALIGN(16) rayTransmissionAttenuationColor;
			cl_float2 ALIGN(8) texcoordScale;
			cl_int ALIGN(4) skipLighting;
			cl_int ALIGN(4) nonShadowing;
			cl_int ALIGN(4) normalInterpolation;
			cl_int ALIGN(4) invertNormal;
			cl_int ALIGN(4) ambientMapTextureIndex;
			cl_int ALIGN(4) diffuseMapTextureIndex;
			cl_float ALIGN(4) specularShininess;
			cl_int ALIGN(4) specularMapTextureIndex;
			cl_int ALIGN(4) emittanceMapTextureIndex;
			cl_int ALIGN(4) fresnelReflection;
			cl_float ALIGN(4) refractiveIndex;
			cl_float ALIGN(4) rayReflectance;
			cl_int ALIGN(4) rayReflectanceMapTextureIndex;
			cl_int ALIGN(4) rayReflectanceGlossinessSampleCountSqrt;
			cl_float ALIGN(4) rayReflectanceGlossiness;
			cl_float ALIGN(4) rayTransmittance;
			cl_int ALIGN(4) rayTransmittanceMapTextureIndex;
			cl_int ALIGN(4) rayTransmittanceGlossinessSampleCountSqrt;
			cl_float ALIGN(4) rayTransmittanceGlossiness;
			cl_int ALIGN(4) enableRayTransmissionAttenuation;
			cl_float ALIGN(4) rayTransmissionAttenuationFactor;
			cl_int ALIGN(4) normalMapTextureIndex;
			cl_int ALIGN(4) maskMapTextureIndex;
			cl_int ALIGN(4) heightMapTextureIndex;
		};

		struct AmbientLight
		{
			cl_float4 ALIGN(16) color;
			cl_float ALIGN(4) intensity;
			cl_int ALIGN(4) enableAmbientOcclusion;
			cl_int ALIGN(4) ambientOcclusionSampleCountSqrt;
			cl_float ALIGN(4) ambientOcclusionMaxSampleDistance;
			cl_float ALIGN(4) ambientOcclusionSampleDistribution;
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
			cl_float ALIGN(4) maxDistance;
			cl_float ALIGN(4) attenuation;
			cl_int ALIGN(4) enableAreaLight;
			cl_int ALIGN(4) areaLightSampleCountSqrt;
			cl_float ALIGN(4) areaLightRadius;
		};

		struct Triangle
		{
			cl_float4 ALIGN(16) vertices[3];
			cl_float4 ALIGN(16) normals[3];
			cl_float4 ALIGN(16) normal;
			cl_float4 ALIGN(16) tangent;
			cl_float4 ALIGN(16) bitangent;
			cl_float2 ALIGN(8) texcoords[3];
			cl_uint ALIGN(4) materialIndex;
		};

		struct AABB
		{
			cl_float4 ALIGN(16) min;
			cl_float4 ALIGN(16) max;
		};

		struct BVHNode
		{
			AABB ALIGN(16) aabb;
			cl_int ALIGN(4) rightOffset;
			cl_int ALIGN(4) startOffset;
			cl_int ALIGN(4) primitiveCount;
		};
	}
}
