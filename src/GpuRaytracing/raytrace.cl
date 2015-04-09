__kernel void raytrace(float alpha, __global float* A, __global float* B, __global float* C)
{
	int index = get_global_id(0);
	C[index] = alpha * A[index] + B[index];
}
