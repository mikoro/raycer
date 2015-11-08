export void test(uniform float vin[], uniform float vout[], uniform int count)
{
    foreach (index = 0 ... count)
	{
		float v = vin[index];

		if (v < 3.0f)
			v = v * v;
		else
			v = sqrt(v);

		vout[index] = v;
    }
}
