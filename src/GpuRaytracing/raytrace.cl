__kernel void raytrace(__write_only image2d_t pixels)
{
	const int2 pos = (int2)(get_global_id(0), get_global_id(1));
	write_imagef(pixels, pos, (float4)((float)pos.x / 1280.0f, (float)pos.y / 800.0f, (float)pos.x / 1280.0f, 1.0f));
}
