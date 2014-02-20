#version 140

in vec2 ex_texture;

uniform sampler2D c3d_mapDiffuse;

out vec4 out_FragColor;

void main()
{
	out_FragColor = texture2D( c3d_mapDiffuse, ex_texture);
}
