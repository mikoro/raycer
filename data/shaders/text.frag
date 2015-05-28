#version 150

uniform sampler2D tex0;

in Data
{
	vec2 texcoord;
	vec4 color;
} input;

out vec4 color;

void main()
{
	float a = texture(tex0, input.texcoord).r;
	color = vec4(input.color.rgb, input.color.a * a);
}
