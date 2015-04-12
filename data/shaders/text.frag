#version 120

uniform sampler2D tex0;

void main()
{
    float a = texture2D(tex0, gl_TexCoord[0].xy).r;
    gl_FragColor = vec4(gl_Color.rgb, gl_Color.a * a);
}
