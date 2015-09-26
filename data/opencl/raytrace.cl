kernel void raytrace(write_only image2d_t image,
	constant State* state,
	constant Camera* camera,
	constant Raytracer* raytracer,
	constant ToneMapper* toneMapper,
	constant SimpleFog* simpleFog,
	constant Material* materials,
	constant AmbientLight* ambientLight,
	constant DirectionalLight* directionalLights,
	constant PointLight* pointLights,
	constant Triangle* triangles,
	constant BVHNode* bvhNodes)
{
	int x = get_global_id(0);
	int y = get_global_id(1);

	Ray ray = getCameraRay(camera, x, y);
	Intersection intersection = constructIntersection();
	float4 finalColor = raytracer->backgroundColor;

	for (int i = 0; i < state->triangleCount; ++i)
		intersectTriangle(&triangles[i], &ray, &intersection, materials);

	if (intersection.wasFound)
	{
		constant Material* material = &materials[intersection.materialIndex];
		finalColor = material->ambientReflectance;
	}

	write_imagef(image, (int2)(x, y), finalColor);
}
