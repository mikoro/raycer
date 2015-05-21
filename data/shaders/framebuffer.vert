#version 150

in vec2 vertex_in;
in vec2 texcoord_in;

out vec2 texcoord;

void main()
{
	gl_Position = vec4(vertex_in, 1.0f, 1.0f);
	texcoord = texcoord_in;
}
