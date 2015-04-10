__kernel void raytrace(__write_only image2d_t pixels)
{
	const int2 pos = (int2)(get_global_id(0), get_global_id(1));
	write_imagef(pixels, pos, (float4)(0.5f, 0.5f, 0.5f, 0.5f));
}
