#version 150

uniform sampler2D tex0;

in Data
{
	vec2 texcoord;
	vec4 color;
} fsin;

out vec4 color;

void main()
{
	float a = texture(tex0, fsin.texcoord).r;
	color = vec4(fsin.color.rgb, fsin.color.a * a);
}
