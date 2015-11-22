float4 getHemisphereSample(ONB onb, float distribution, float x, float y)
{
	float phi = 2.0 * M_PI * x;
	float cos_phi = cos(phi);
	float sin_phi = sin(phi);
	float cos_theta = pow(1.0 - y, 1.0 / (distribution + 1.0));
	float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

	float u = sin_theta * cos_phi;
	float v = sin_theta * sin_phi;
	float w = cos_theta;

	return u * onb.u + v * onb.v + w * onb.w;
}
