#version 330

in vec2 vTexcoord;
out vec4 fColor;

uniform sampler2D texture0;

void main()
{
	fColor = texture(texture0, vTexcoord);
}
