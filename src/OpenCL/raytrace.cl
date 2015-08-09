kernel void raytrace(constant Info* info, constant Camera* camera, constant Light* lights, constant Plane* planes, constant Sphere* spheres, write_only image2d_t pixels)
{
	float x = (float)get_global_id(0);
	float y = (float)get_global_id(1);
	float k = fabs(cos(x + info->time)) * fabs(cos(y + info->time));
	float4 color = (float4)(fabs(cos(info->time)) * k, fabs(cos(info->time + 0.5f)) * k, fabs(cos(info->time + 1.0f)) * k, 1.0f);

	write_imagef(pixels, (int2)(x, y), color);
}
