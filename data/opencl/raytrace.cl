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
	float4 finalColor = raytracer->backgroundColor;

	intersectBVH(nodes, triangles, materials, &ray, &intersection);

	if (intersection.wasFound)
	{
		constant Material* material = &materials[intersection.materialIndex];

		if (material->diffuseMapTextureIndex != -1)
		{
			finalColor = getTextureColor(material->diffuseMapTextureIndex, intersection.texcoord, TEX_OUTPUT_ARGS);
		}
		else
			finalColor = material->diffuseReflectance;
	}

	write_imagef(outputImage, (int2)(x, y), clamp(pow(finalColor, 1.0 / 2.2), 0.0, 1.0));
}
