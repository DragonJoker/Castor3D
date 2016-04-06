#version 420

in vec4 geo_color;

out vec4 out_FragColor;

void main()
{
	out_FragColor = geo_color;
}
