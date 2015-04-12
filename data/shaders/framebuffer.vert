#version 120

attribute vec2 vertex_in;
attribute vec2 tex_coord_in;

varying vec2 tex_coord;

void main()
{
	gl_Position = vec4(vertex_in, 1.0f, 1.0f);
	tex_coord = tex_coord_in;
}
