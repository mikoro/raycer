kernel void pathtrace(
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

	//Ray ray = getCameraRay(camera, x, y);
	//Intersection intersection = constructIntersection();
	//float4 outputColor = general->backgroundColor;

	cumulativeColors[index] += (float4)(0.2, 0.4, 0.6, 1.0);
	filterWeights[index] += 1.0f;
}
