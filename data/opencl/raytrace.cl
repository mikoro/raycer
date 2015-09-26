kernel void raytrace(
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
	constant BVHNode* bvhNodes,
	write_only image2d_t outputImage,
	read_only image2d_t texture0)
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

	//const sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
	//finalColor = read_imagef(texture0, sampler, (float2)((float)x / camera->imagePlaneWidth, (float)y / camera->imagePlaneHeight));

	write_imagef(outputImage, (int2)(x, y), clamp(pow(finalColor, 1.0 / 2.2), 0.0, 1.0));
}
