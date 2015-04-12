#version 120

uniform sampler2D tex0;

varying vec2 tex_coord;

void main()
{
	gl_FragColor = texture2D(tex0, tex_coord);
}
