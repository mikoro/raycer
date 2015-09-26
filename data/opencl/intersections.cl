bool intersectAABB(constant AABB* aabb, Ray* ray)
{
	float tx0 = (aabb->min.x - ray->origin.x) * ray->inverseDirection.x;
	float tx1 = (aabb->max.x - ray->origin.x) * ray->inverseDirection.x;

	float tmin = min(tx0, tx1);
	float tmax = max(tx0, tx1);

	float ty0 = (aabb->min.y - ray->origin.y) * ray->inverseDirection.y;
	float ty1 = (aabb->max.y - ray->origin.y) * ray->inverseDirection.y;

	tmin = max(tmin, min(ty0, ty1));
	tmax = min(tmax, max(ty0, ty1));

	float tz0 = (aabb->min.z - ray->origin.z) * ray->inverseDirection.z;
	float tz1 = (aabb->max.z - ray->origin.z) * ray->inverseDirection.z;

	tmin = max(tmin, min(tz0, tz1));
	tmax = min(tmax, max(tz0, tz1));

	if (tmax < max(tmin, 0.0))
		return false;

	return true;
}

bool intersectTriangle(constant Triangle* triangle, constant Material* materials, Ray* ray, Intersection* intersection)
{
	constant Material* material = &materials[triangle->materialIndex];

	if (ray->isShadowRay && material->nonShadowing)
		return false;

	if (ray->fastOcclusion && intersection->wasFound)
		return true;

	float4 v0v1 = triangle->vertices[1] - triangle->vertices[0];
	float4 v0v2 = triangle->vertices[2] - triangle->vertices[0];

	float4 pvec = cross(ray->direction, v0v2);
	float determinant = dot(v0v1, pvec);

	if (fabs(determinant) < FLT_EPSILON)
		return false;

	float invDeterminant = 1.0 / determinant;

	float4 tvec = ray->origin - triangle->vertices[0];
	float u = dot(tvec, pvec) * invDeterminant;

	if (u < 0.0 || u > 1.0)
		return false;

	float4 qvec = cross(tvec, v0v1);
	float v = dot(ray->direction, qvec) * invDeterminant;

	if (v < 0.0 || (u + v) > 1.0)
		return false;

	float t = dot(v0v2, qvec) * invDeterminant;

	if (t < 0.0)
		return false;

	if (t < ray->minDistance || t > ray->maxDistance)
		return false;

	if (t > intersection->distance)
		return false;

	float w = 1.0 - u - v;

	float2 texcoord = (w * triangle->texcoords[0] + u * triangle->texcoords[1] + v * triangle->texcoords[2]) * material->texcoordScale;
	float4 ip = ray->origin + (t * ray->direction);

	texcoord.x = texcoord.x - floor(texcoord.x);
	texcoord.y = texcoord.y - floor(texcoord.y);

	//if (material->maskMapTexture != NULL)
	//{
	//	if (material->maskMapTexture->getValue(texcoord, ip) < 0.5)
	//		return false;
	//}

	float4 finalNormal = material->normalInterpolation ? (w * triangle->normals[0] + u * triangle->normals[1] + v * triangle->normals[2]) : triangle->normal;

	intersection->wasFound = true;
	intersection->distance = t;
	intersection->position = ip;
	intersection->normal = material->invertNormal ? -finalNormal : finalNormal;
	intersection->onb = constructONBFromValues(triangle->tangent, triangle->bitangent, intersection->normal);
	intersection->texcoord = texcoord;
	intersection->materialIndex = triangle->materialIndex;

	return true;
}

bool intersectBVH(constant BVHNode* nodes, constant Triangle* triangles, constant Material* materials, Ray* ray, Intersection* intersection)
{
	int stack[64];
	int stackptr = 0;
	bool wasFound = false;

	// push to stack
	stack[stackptr] = 0;
	stackptr++;

	while (stackptr > 0)
	{
		// pop from stack
		stackptr--;
		int index = stack[stackptr];
		BVHNode node = nodes[index];

		// leaf node -> intersect with all its primitives
		if (node.rightOffset == 0)
		{
			for (int i = 0; i < node.primitiveCount; ++i)
			{
				if (intersectTriangle(&triangles[node.startOffset + i], materials, ray, intersection))
				{
					if (ray->fastOcclusion)
						return true;

					wasFound = true;
				}
			}
		}
		else // travel down the tree
		{
			// right child
			if (intersectAABB(&nodes[index + node.rightOffset].aabb, ray))
			{
				stack[stackptr] = index + node.rightOffset;
				stackptr++;
			}

			// left child
			if (intersectAABB(&nodes[index + 1].aabb, ray))
			{
				stack[stackptr] = index + 1;
				stackptr++;
			}
		}
	}

	return wasFound;
}
