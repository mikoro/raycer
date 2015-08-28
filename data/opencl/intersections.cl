void intersectPlane(constant Plane* plane, Ray* ray, Intersection* intersection, constant Material* materials)
{
	float denominator = dot(ray->direction, plane->normal);

	if (fabs(denominator) < FLT_EPSILON)
		return;

	float t = dot(plane->position - ray->origin, plane->normal) / denominator;

	if (t < 0.0 || t < ray->minDistance || t > ray->maxDistance)
		return;

	if (t > intersection->distance)
		return;

	float4 ip = ray->origin + (t * ray->direction);

	intersection->wasFound = true;
	intersection->distance = t;
	intersection->position = ip;
	intersection->onb = constructONBFromNormal(plane->normal);
	intersection->materialIndex = plane->materialIndex;

	constant Material* material = &materials[plane->materialIndex];

	float u = dot(plane->uAxis, ip) / material->texcoordScale.x;
	float v = dot(plane->vAxis, ip) / material->texcoordScale.y;

	intersection->texcoord.x = fabs(u - floor(u));
	intersection->texcoord.y = fabs(v - floor(v));
}
