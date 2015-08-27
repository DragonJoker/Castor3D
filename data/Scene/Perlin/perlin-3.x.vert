#version 140

in vec4 vertex;
in vec2 texture;

out vec2 vtx_texture;

uniform mat4 ProjectionModelViewMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;

void main()
{
	vtx_texture = texture.xy;

	gl_Position = ProjectionModelViewMatrix * vertex;
}