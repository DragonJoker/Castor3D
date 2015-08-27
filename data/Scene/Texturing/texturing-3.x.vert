#version 140

in vec4 vertex;
in vec2 texture;

uniform mat4 	c3d_mtxProjection;
uniform mat4 	c3d_mtxModel;
uniform mat4 	c3d_mtxView;

out vec2 ex_texture;

void main()
{
	ex_texture = texture.xy;

	gl_Position = c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vertex;
}