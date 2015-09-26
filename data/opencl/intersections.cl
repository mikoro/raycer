void intersectTriangle(constant Triangle* triangle, Ray* ray, Intersection* intersection, constant Material* materials)
{
	float4 v0v1 = triangle->vertices[1] - triangle->vertices[0];
	float4 v0v2 = triangle->vertices[2] - triangle->vertices[0];

	float4 pvec = cross(ray->direction, v0v2);
	float determinant = dot(v0v1, pvec);

	if (fabs(determinant) < FLT_EPSILON)
		return;

	float invDeterminant = 1.0 / determinant;

	float4 tvec = ray->origin - triangle->vertices[0];
	float u = dot(tvec, pvec) * invDeterminant;

	if (u < 0.0 || u > 1.0)
		return;

	float4 qvec = cross(tvec, v0v1);
	float v = dot(ray->direction, qvec) * invDeterminant;

	if (v < 0.0 || (u + v) > 1.0)
		return;

	float t = dot(v0v2, qvec) * invDeterminant;

	if (t < 0.0 || t < ray->minDistance || t > ray->maxDistance)
		return;

	if (t > intersection->distance)
		return;

	float w = 1.0 - u - v;

	//float4 interpolatedNormal = w * triangle->normals[0] + u * triangle->normals[1] + v * triangle->normals[2];
	float2 interpolatedTexcoord = w * triangle->texcoords[0] + u * triangle->texcoords[1] + v * triangle->texcoords[2];

	constant Material* material = &materials[triangle->materialIndex];

	intersection->wasFound = true;
	intersection->distance = t;
	intersection->position = ray->origin + (t * ray->direction);
	intersection->onb = constructONBFromValues(triangle->tangent, triangle->bitangent, triangle->normal);
	intersection->texcoord = interpolatedTexcoord / material->texcoordScale;
	intersection->materialIndex = triangle->materialIndex;
}
