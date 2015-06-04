#version 150

uniform sampler2D texture0;

in Data
{
	vec2 texcoord;
} fsin;

out vec3 color;

void main()
{
	color = texture(texture0, fsin.texcoord).rgb;
}
