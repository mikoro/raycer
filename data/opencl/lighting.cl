float4 calculatePhongShadingColor(float4 normal,
	float4 directionToLight,
	float4 directionToCamera,
	float4 lightColor,
	float lightIntensity,
	float4 diffuseReflectance,
	float4 specularReflectance,
	float specularShininess)
{
	float4 phongShadingColor = (float4)(0.0, 0.0, 0.0, 0.0);

	float diffuseAmount = dot(directionToLight, normal);

	if (diffuseAmount > 0.0)
	{
		phongShadingColor = lightColor * lightIntensity * diffuseAmount * diffuseReflectance;

		float4 reflectionDirection = normalize((2.0 * diffuseAmount * normal) - directionToLight);
		float specularAmount = dot(reflectionDirection, directionToCamera);

		if (specularAmount > 0.0)
			phongShadingColor += lightColor * lightIntensity * pow(specularAmount, specularShininess) * specularReflectance;
	}

	return phongShadingColor;
}

float calculateShadowAmount(constant BVHNode* nodes,
	constant Triangle* triangles,
	constant Material* materials,
	constant General* general,
	float4 position,
	float4 directionToLight,
	float maxDistance,
	TEX_INPUT_ARGS)
{
	Ray shadowRay = constructRay(position + directionToLight * general->rayStartOffset, directionToLight);
	Intersection shadowIntersection = constructIntersection();

	shadowRay.isShadowRay = true;
	shadowRay.fastOcclusion = true;
	shadowRay.maxDistance = maxDistance;

	if (intersectBVH(nodes, triangles, materials, shadowRay, &shadowIntersection, TEX_OUTPUT_ARGS))
		return 1.0;
	else
		return 0.0;
}

float4 calculateLightColor(constant BVHNode* nodes,
	constant Triangle* triangles,
	constant Material* materials,
	constant State* state,
	constant General* general,
	constant AmbientLight* ambientLight,
	constant DirectionalLight* directionalLights,
	constant PointLight* pointLights,
	Ray ray,
	Intersection intersection,
	TEX_INPUT_ARGS)
{
	float4 lightColor = (float4)(0.0, 0.0, 0.0, 0.0);
	float4 directionToCamera = -ray.direction;

	Material material = materials[intersection.materialIndex];

	float4 mappedAmbientReflectance = (float4)(1.0, 1.0, 1.0, 1.0);
	float4 mappedDiffuseReflectance = (float4)(1.0, 1.0, 1.0, 1.0);
	float4 mappedSpecularReflectance = (float4)(1.0, 1.0, 1.0, 1.0);

	if (material.ambientMapTextureIndex != -1)
		mappedAmbientReflectance = getTextureColor(material.ambientMapTextureIndex, intersection.texcoord, TEX_OUTPUT_ARGS);

	if (material.skipLighting)
		return mappedAmbientReflectance;

	if (material.diffuseMapTextureIndex != -1)
		mappedDiffuseReflectance = getTextureColor(material.diffuseMapTextureIndex, intersection.texcoord, TEX_OUTPUT_ARGS);

	if (material.specularMapTextureIndex != -1)
		mappedSpecularReflectance = getTextureColor(material.specularMapTextureIndex, intersection.texcoord, TEX_OUTPUT_ARGS);

	float4 finalAmbientReflectance = material.ambientReflectance * mappedAmbientReflectance;
	float4 finalDiffuseReflectance = material.diffuseReflectance * mappedDiffuseReflectance;
	float4 finalSpecularReflectance = material.specularReflectance * mappedSpecularReflectance;

	lightColor += ambientLight->color * ambientLight->intensity * finalAmbientReflectance;

	for (int i = 0; i < state->directionalLightCount; ++i)
	{
		DirectionalLight directionalLight = directionalLights[i];
		float4 directionToLight = -directionalLight.direction;

		float4 directionalLightColor = calculatePhongShadingColor(intersection.normal,
			directionToLight,
			directionToCamera,
			directionalLight.color,
			directionalLight.intensity,
			finalDiffuseReflectance,
			finalSpecularReflectance,
			material.specularShininess);

		float shadowAmount = calculateShadowAmount(nodes,
			triangles,
			materials,
			general,
			intersection.position,
			directionToLight,
			FLT_MAX,
			TEX_OUTPUT_ARGS);

		lightColor += directionalLightColor * (1.0 - shadowAmount);
	}

	for (int i = 0; i < state->pointLightCount; ++i)
	{
		PointLight pointLight = pointLights[i];

		float4 directionToLight = (pointLight.position - intersection.position);
		float distanceToLight = length(directionToLight);
		directionToLight = normalize(directionToLight);

		float4 pointLightColor = calculatePhongShadingColor(intersection.normal,
			directionToLight,
			directionToCamera,
			pointLight.color,
			pointLight.intensity,
			finalDiffuseReflectance,
			finalSpecularReflectance,
			material.specularShininess);

		float shadowAmount = calculateShadowAmount(nodes,
			triangles,
			materials,
			general,
			intersection.position,
			directionToLight,
			distanceToLight,
			TEX_OUTPUT_ARGS);

		float distanceAttenuation = min(1.0, distanceToLight / pointLight.maxDistance);
		distanceAttenuation = 1.0 - pow(distanceAttenuation, pointLight.attenuation);

		lightColor += pointLightColor * distanceAttenuation * (1.0 - shadowAmount);
	}

	return lightColor;
}
