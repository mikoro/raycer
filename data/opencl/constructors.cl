ONB constructONB()
{
	ONB onb;

	onb.u = (float4)(0.0, 0.0, 0.0, 0.0);
	onb.v = (float4)(0.0, 0.0, 0.0, 0.0);
	onb.w = (float4)(0.0, 0.0, 0.0, 0.0);

	return onb;
}

ONB constructONBFromValues(float4 u, float4 v, float4 w)
{
	ONB onb;

	onb.u = u;
	onb.v = v;
	onb.w = w;

	return onb;
}

ONB constructONBFromNormal(float4 normal)
{
	ONB onb;

	onb.w = normal;
	onb.v = normalize(cross(normal, (float4)(0.0001, 1.0, 0.0001, 0.0)));
	onb.u = normalize(cross(onb.v, normal));

	return onb;
}

Ray constructRay(float4 origin, float4 direction)
{
	Ray ray;

	ray.origin = origin;
	ray.direction = direction;
	ray.inverseDirection = (float4)(1.0, 1.0, 1.0, 1.0) / direction;
	ray.minDistance = 0.0;
	ray.maxDistance = FLT_MAX;
	ray.time = 0.0;
	ray.isShadowRay = false;
	ray.fastOcclusion = false;

	return ray;
}

Intersection constructIntersection()
{
	Intersection intersection;

	intersection.wasFound = false;
	intersection.distance = FLT_MAX;
	intersection.position = (float4)(0.0, 0.0, 0.0, 0.0);
	intersection.normal = (float4)(0.0, 0.0, 0.0, 0.0);
	intersection.onb = constructONB();
	intersection.texcoord = (float2)(0.0, 0.0);
	intersection.materialIndex = 0;

	return intersection;
}
