// Structs for external data (remember to sync changes) //

typedef struct State
{
	float time;
	int directionalLightCount;
	int pointLightCount;
	int triangleCount;
	int bvhNodeCount;
} State;

typedef struct General
{
	float4 backgroundColor;
	float4 offLensColor;
	float rayStartOffset;
	int maxRayIterations;
	int maxPathLength;
	int pathSampleCount;
	int multiSampleCountSqrt;
	int timeSampleCount;
	int cameraSampleCountSqrt;
	int visualizeDepth;
	float visualizeDepthMaxDistance;
	int enableNormalMapping;
} General;

typedef struct Camera
{
	float4 position;
	float4 forward;
	float4 right;
	float4 up;
	float4 imagePlaneCenter;
	float4 translateInTime;
	float4 rotateInTime;
	int projectionType;
	int isTimeVariant;
	int hasMoved;
	float fov;
	float orthoSize;
	float fishEyeAngle;
	float apertureSize;
	float focalDistance;
	float aspectRatio;
	float imagePlaneDistance;
	float imagePlaneWidth;
	float imagePlaneHeight;
} Camera;

typedef struct ToneMapper
{
	int type;
	int applyGamma;
	int shouldClamp;
	float gamma;
	float exposure;
	float key;
	int enableAveraging;
	float averagingAlpha;
} ToneMapper;

typedef struct SimpleFog
{
	float4 color;
	int enabled;
	float distance;
	float steepness;
	int heightDispersion;
	float height;
	float heightSteepness;
} SimpleFog;

typedef struct Material
{
	float4 ambientReflectance;
	float4 diffuseReflectance;
	float4 specularReflectance;
	float4 emittance;
	float4 rayTransmissionAttenuationColor;
	float2 texcoordScale;
	int skipLighting;
	int nonShadowing;
	int normalInterpolation;
	int invertNormal;
	int ambientMapTextureIndex;
	int diffuseMapTextureIndex;
	float specularShininess;
	int specularMapTextureIndex;
	int emittanceMapTextureIndex;
	int fresnelReflection;
	float refractiveIndex;
	float rayReflectance;
	int rayReflectanceMapTextureIndex;
	int rayReflectanceGlossinessSampleCountSqrt;
	float rayReflectanceGlossiness;
	float rayTransmittance;
	int rayTransmittanceMapTextureIndex;
	int rayTransmittanceGlossinessSampleCountSqrt;
	float rayTransmittanceGlossiness;
	int enableRayTransmissionAttenuation;
	float rayTransmissionAttenuationFactor;
	int normalMapTextureIndex;
	int maskMapTextureIndex;
	int heightMapTextureIndex;
} Material;

typedef struct AmbientLight
{
	float4 color;
	float intensity;
	int enableAmbientOcclusion;
	int ambientOcclusionSampleCountSqrt;
	float ambientOcclusionMaxSampleDistance;
	float ambientOcclusionSampleDistribution;
} AmbientLight;

typedef struct DirectionalLight
{
	float4 color;
	float4 direction;
	float intensity;
} DirectionalLight;

typedef struct PointLight
{
	float4 color;
	float4 position;
	float intensity;
	float maxDistance;
	float attenuation;
	int enableAreaLight;
	int areaLightSampleCountSqrt;
	float areaLightRadius;
} PointLight;

typedef struct Triangle
{
	float4 vertices[3];
	float4 normals[3];
	float4 normal;
	float4 tangent;
	float4 bitangent;
	float2 texcoords[3];
	int materialIndex;
} Triangle;

typedef struct AABB
{
	float4 min;
	float4 max;
} AABB;

typedef struct BVHNode
{
	AABB aabb;
	int rightOffset;
	int startOffset;
	int primitiveCount;
} BVHNode;

// Structs for internal data (no need to sync)  //

typedef struct ONB
{
	float4 u;
	float4 v;
	float4 w;
} ONB;

typedef struct Ray
{
	float4 origin;
	float4 direction;
	float4 inverseDirection;
	float minDistance;
	float maxDistance;
	float time;
	bool isShadowRay;
	bool fastOcclusion;
} Ray;

typedef struct Intersection
{
	bool wasFound;
	float distance;
	float4 position;
	float4 normal;
	ONB onb;
	float2 texcoord;
	int materialIndex;
} Intersection;
