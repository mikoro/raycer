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

void intersectSphere(constant Sphere* sphere, Ray* ray, Intersection* intersection, constant Material* materials)
{
	float4 rayOriginToSphere = sphere->position - ray->origin;
	float rayOriginToSphereDistance2 = length(rayOriginToSphere);
	rayOriginToSphereDistance2 = rayOriginToSphereDistance2 * rayOriginToSphereDistance2;

	float t1 = dot(rayOriginToSphere, ray->direction);
	float sphereToRayDistance2 = rayOriginToSphereDistance2 - (t1 * t1);
	float radius2 = sphere->radius * sphere->radius;

	bool rayOriginIsOutside = (rayOriginToSphereDistance2 >= radius2);

	if (rayOriginIsOutside)
	{
		if (t1 < 0.0)
			return;

		if (sphereToRayDistance2 > radius2)
			return;
	}

	float t2 = sqrt(radius2 - sphereToRayDistance2);
	float t = (rayOriginIsOutside) ? (t1 - t2) : (t1 + t2);

	if (t < ray->minDistance || t > ray->maxDistance)
		return;

	if (t > intersection->distance)
		return;

	float4 ip = ray->origin + (t * ray->direction);
	float4 normal = normalize(ip - sphere->position);

	intersection->wasFound = true;
	intersection->distance = t;
	intersection->position = ip;
	intersection->onb = constructONBFromNormal(normal);
	intersection->materialIndex = sphere->materialIndex;

	constant Material* material = &materials[sphere->materialIndex];

	float u = 0.5 + atan2(normal.z, normal.x) / (2.0 * M_PI);
	float v = 0.5 - asin(normal.y) / M_PI;

	u /= material->texcoordScale.x;
	v /= material->texcoordScale.y;

	intersection->texcoord.x = u - floor(u);
	intersection->texcoord.y = v - floor(v);
}

void intersectBox(constant Box* box, Ray* ray, Intersection* intersection, constant Material* materials)
{
	float4 vmin = box->position - box->extent / 2.0;
	float4 vmax = box->position + box->extent / 2.0;

	float tx0 = (vmin.x - ray->origin.x) * ray->inverseDirection.x;
	float tx1 = (vmax.x - ray->origin.x) * ray->inverseDirection.x;

	float tmin = min(tx0, tx1);
	float tmax = max(tx0, tx1);

	float4 minNormal = (float4)(1.0, 0.0, 0.0, 0.0) * sign(ray->direction.x);
	float4 maxNormal = (float4)(1.0, 0.0, 0.0, 0.0) * sign(ray->direction.x);

	float ty0 = (vmin.y - ray->origin.y) * ray->inverseDirection.y;
	float ty1 = (vmax.y - ray->origin.y) * ray->inverseDirection.y;
	float tymin = min(ty0, ty1);
	float tymax = max(ty0, ty1);

	if (tymin > tmin)
	{
		minNormal = (float4)(0.0, 1.0, 0.0, 0.0) * sign(ray->direction.y);
		tmin = tymin;
	}

	if (tymax < tmax)
	{
		maxNormal = (float4)(0.0, 1.0, 0.0, 0.0) * sign(ray->direction.y);
		tmax = tymax;
	}

	float tz0 = (vmin.z - ray->origin.z) * ray->inverseDirection.z;
	float tz1 = (vmax.z - ray->origin.z) * ray->inverseDirection.z;
	float tzmin = min(tz0, tz1);
	float tzmax = max(tz0, tz1);

	if (tzmin > tmin)
	{
		minNormal = (float4)(0.0, 0.0, 1.0, 0.0) * sign(ray->direction.z);
		tmin = tzmin;
	}

	if (tzmax < tmax)
	{
		maxNormal = (float4)(0.0, 0.0, 1.0, 0.0) * sign(ray->direction.z);
		tmax = tzmax;
	}

	if (tmax < max(tmin, 0.0))
		return;

	bool isInside = (tmin < 0.0);
	float t = isInside ? tmax : tmin;

	if (t < ray->minDistance || t > ray->maxDistance)
		return;

	if (t > intersection->distance)
		return;

	intersection->wasFound = true;
	intersection->distance = t;
	intersection->position = ray->origin + (t * ray->direction);
	intersection->onb = constructONBFromNormal(isInside ? maxNormal : -minNormal);
	intersection->materialIndex = box->materialIndex;
}

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
