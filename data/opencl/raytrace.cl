kernel void raytrace(
	constant State* state,
	constant General* general,
	constant Camera* camera,
	constant ToneMapper* toneMapper,
	constant SimpleFog* simpleFog,
	constant Material* materials,
	constant AmbientLight* ambientLight,
	constant DirectionalLight* directionalLights,
	constant PointLight* pointLights,
	constant Triangle* triangles,
	constant BVHNode* nodes,
	global uint2* seeds,
	global float4* cumulativeColors,
	global float* filterWeights,
	TEX_INPUT_ARGS)
{
	int x = get_global_id(0);
	int y = get_global_id(1);
	int index = y * state->imageWidth + x;

	Ray ray = getCameraRay(camera, x, y);
	Intersection intersection = constructIntersection();
	float4 finalColor = general->backgroundColor;

	if (state->triangleCount > 0 && state->bvhNodeCount > 0)
	{
		if (intersectBVH(nodes, triangles, materials, ray, &intersection, TEX_OUTPUT_ARGS))
		{
			finalColor = calculateLightColor(nodes,
				triangles,
				materials,
				state,
				general,
				ambientLight,
				directionalLights,
				pointLights,
				ray,
				intersection,
				TEX_OUTPUT_ARGS);
		}
	}

	cumulativeColors[index] += finalColor;
	filterWeights[index] += 1.0f;
}
