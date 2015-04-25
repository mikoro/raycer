typedef struct Info
{
	float width;
	float height;
	float time;
	int lightCount;
	int planeCount;
	int sphereCount;
} Info;

typedef struct Camera
{
	float4 position;
	float4 up;
	float4 right;
	float4 imagePlaneCenter;
	float imagePlaneWidth;
	float imagePlaneHeight;
	float aspectRatio;
} Camera;

typedef struct Light
{
	float4 position;
	float4 color;
	float intensity;
} Light;

typedef struct Plane
{
	float4 position;
	float4 normal;
	float4 color;
} Plane;

typedef struct Sphere
{
	float4 position;
	float4 color;
	float radius;
	float radius2;
} Sphere;

__kernel void printStructSizes()
{
	printf("Info: %d\n", sizeof(Info));
	printf("Camera: %d\n", sizeof(Camera));
	printf("Light: %d\n", sizeof(Light));
	printf("Plane: %d\n", sizeof(Plane));
	printf("Sphere: %d\n", sizeof(Sphere));
}
