// http://cas.ee.ic.ac.uk/people/dt10/research/rngs-gpu-mwc64x.html
uint randomInt(uint2* state)
{
	enum { A = 4294883355U };
	uint x = (*state).x;
	uint c = (*state).y;
	uint res = x ^ c;
	uint hi = mul_hi(x, A);
	x = x * A + c;
	c = hi + (x < c);
	*state = (uint2)(x, c);

	return res;
}

float randomFloat(uint2* state)
{
	return (float)randomInt(state) / (float)0xFFFFFFFF;
}
