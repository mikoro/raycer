#version 150

uniform sampler2D tex0;

in vec2 texcoord;

out vec4 color_out;

void main()
{
	color_out = texture(tex0, texcoord);
}
