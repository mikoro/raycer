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
	if (state->triangleCount == 0 || state->bvhNodeCount == 0)
		return;

	int x = get_global_id(0);
	int y = get_global_id(1);
	int index = y * state->imageWidth + x;

	Ray ray;
	Intersection intersection;

	for (int i = 0; i < general->pathSampleCount; ++i)
	{
		ray = getCameraRay(camera, x, y);
		float4 sampleColor = (float4)(1.0, 1.0, 1.0, 0.0);
		bool emittanceFound = false;

		for (int j = 0; j < general->maxPathLength; ++j)
		{
			intersection = constructIntersection();

			if (intersectBVH(nodes, triangles, materials, ray, &intersection, TEX_OUTPUT_ARGS))
			{
				Material material = materials[intersection.materialIndex];

				if (material.isEmissive)
				{
					float4 emittance = material.emittance;

					if (material.emittanceMapTextureIndex != -1)
						emittance = getTextureColor(material.emittanceMapTextureIndex, intersection.texcoord, TEX_OUTPUT_ARGS);

					sampleColor *= emittance;
					emittanceFound = true;
					break;
				}

				float rx = randomFloat(&seeds[index]);
				float ry = randomFloat(&seeds[index]);

				float4 newDirection = getHemisphereSample(intersection.onb, 1.0, rx, ry);
				ray = constructRay(intersection.position + general->rayStartOffset * newDirection, newDirection);

				float4 reflectance = material.diffuseReflectance;

				if (material.diffuseMapTextureIndex != -1)
					reflectance = getTextureColor(material.diffuseMapTextureIndex, intersection.texcoord, TEX_OUTPUT_ARGS);

				float alpha = fabs(dot(newDirection, intersection.normal));
				float4 brdf = 2.0 * reflectance * alpha;

				sampleColor *= brdf;
			}
			else
				break;
		}

		if (!emittanceFound)
			sampleColor = (float4)(0.0, 0.0, 0.0, 0.0);

		cumulativeColors[index] += sampleColor;
		filterWeights[index] += 1.0;
	}
}
