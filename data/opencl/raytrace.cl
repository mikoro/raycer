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
	constant BVHNode* nodes,
	write_only image2d_t outputImage,
	TEX_INPUT_ARGS)
{
	int x = get_global_id(0);
	int y = get_global_id(1);

	Ray ray = getCameraRay(camera, x, y);
	Intersection intersection = constructIntersection();
	float4 outputColor = raytracer->backgroundColor;

	if (intersectBVH(nodes, triangles, materials, ray, &intersection, TEX_OUTPUT_ARGS))
	{
		outputColor = calculateLightColor(nodes,
			triangles,
			materials,
			state,
			raytracer,
			ambientLight,
			directionalLights,
			pointLights,
			ray,
			intersection,
			TEX_OUTPUT_ARGS);
	}

	outputColor = clamp(pow(outputColor, 1.0 / 2.2), 0.0, 1.0);
	outputColor.w = 1.0;

	write_imagef(outputImage, (int2)(x, y), outputColor);
}
