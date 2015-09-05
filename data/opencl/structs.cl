typedef struct State
{
	float time;
	int directionalLightCount;
	int pointLightCount;
	int spotLightCount;
	int planeCount;
	int sphereCount;
	int boxCount;
	int triangleCount;
} State;

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
	float imagePlaneWidth;
	float imagePlaneHeight;
	float imagePlaneDistance;
} Camera;

typedef struct Raytracer
{
	float4 backgroundColor;
	float4 offLensColor;
	int maxRayIterations;
	float rayStartOffset;
	int multiSamples;
	int timeSamples;
	int cameraSamples;
} Raytracer;

typedef struct ToneMapper
{
	int type;
	int applyGamma;
	int shouldClamp;
	float gamma;
	float exposure;
	float key;
	float maxLuminance;
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
	float4 color;
	float4 ambientReflectance;
	float4 diffuseReflectance;
	float4 specularReflectance;
	float4 attenuationColor;
	float2 texcoordScale;
	float colorIntensity;
	float shininess;
	int skipLighting;
	int nonShadowing;
	int normalInterpolation;
	int backfaceCulling;
	int invertNormal;
	int hasTexture;
	int textureIndex;
	float rayReflectance;
	float rayTransmittance;
	float refractiveIndex;
	int isFresnel;
	int enableAttenuation;
	float attenuation;
	int id;
} Material;

typedef struct AmbientLight
{
	float4 color;
	float intensity;
	int enableOcclusion;
	float maxDistance;
	int samplerType;
	int samples;
	float distribution;
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
	float distance;
	float attenuation;
	int softShadows;
	float radius;
	int samplerType;
	int samples;
} PointLight;

typedef struct SpotLight
{
	float4 color;
	float4 position;
	float4 direction;
	float intensity;
	float distance;
	float attenuation;
	float sideAttenuation;
	float angle;
	int softShadows;
	float radius;
	int samplerType;
	int samples;
} SpotLight;

typedef struct Plane
{
	float4 position;
	float4 normal;
	float4 uAxis;
	float4 vAxis;
	int invisible;
	int materialIndex;
} Plane;

typedef struct Sphere
{
	float4 position;
	float radius;
	int invisible;
	int materialIndex;
} Sphere;

typedef struct Box
{
	float4 position;
	float4 extent;
	int invisible;
	int materialIndex;
} Box;

typedef struct Triangle
{
	float4 vertices[3];
	float4 normals[3];
	float4 normal;
	float4 tangent;
	float4 bitangent;
	float2 texcoords[3];
	int invisible;
	int materialIndex;
} Triangle;

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
	bool fastOcclusion;
	bool isShadowRay;
	bool isInvalid;
} Ray;

typedef struct Intersection
{
	bool wasFound;
	float distance;
	float4 position;
	ONB onb;
	float2 texcoord;
	int materialIndex;
} Intersection;
