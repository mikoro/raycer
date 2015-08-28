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

Ray constructRay(float4 origin, float4 direction)
{
	Ray ray;

	ray.origin = origin;
	ray.direction = direction;
	ray.inverseDirection = (float4)(1.0, 1.0, 1.0, 1.0) / direction;
	ray.minDistance = 0.0;
	ray.maxDistance = FLT_MAX;
	ray.time = 0.0;
	ray.fastOcclusion = false;
	ray.isShadowRay = false;
	ray.isInvalid = false;

	return ray;
}

kernel void main(write_only image2d_t image,
	constant State* state,
	constant Camera* camera,
	constant Raytracer* raytracer,
	constant ToneMapper* toneMapper,
	constant SimpleFog* simpleFog,
	constant Material* materials,
	constant AmbientLight* ambientLight,
	constant DirectionalLight* directionalLights,
	constant PointLight* pointLights,
	constant SpotLight* spotLights,
	constant Plane* planes,
	constant Sphere* spheres,
	constant Box* boxes,
	constant Triangle* triangles)
{
	float x = (float)get_global_id(0);
	float y = (float)get_global_id(1);
	float k = fabs(cos(x + state->time)) * fabs(cos(y + state->time));
	float4 color = (float4)(fabs(cos(state->time)) * k, fabs(cos(state->time + 0.5f)) * k, fabs(cos(state->time + 1.0f)) * k, 1.0f);

	write_imagef(image, (int2)(x, y), color);
}
