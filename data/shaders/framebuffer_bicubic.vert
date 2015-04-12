#version 330

in vec2 vertex_in;
in vec2 tex_coord_in;

out vec2 tex_coord;

void main()
{
	gl_Position = vec4(vertex_in, 1.0f, 1.0f);
	tex_coord = tex_coord_in;
}
