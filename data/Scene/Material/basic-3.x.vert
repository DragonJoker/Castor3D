#version 140
precision highp float;

in 		vec4 	vertex;
in 		vec3 	normal;
in 		vec2 	texture;

uniform mat4 	c3d_mtxProjectionModelView;
uniform mat4 	c3d_mtxModelView;
uniform mat3 	c3d_mtxNormal;

out		vec2 	vtx_texture;
out 	vec3 	vtx_normal;
out 	vec3 	vtx_vertex;
out 	mat4 	vtx_mtxModelView;

void main()
{
	vtx_mtxModelView 	= c3d_mtxModelView;
	vtx_normal 			= c3d_mtxNormal * normal;
	vtx_vertex 			= (vtx_mtxModelView * vertex).xyz;
	vtx_texture			= texture.xy;

	gl_Position = c3d_mtxProjectionModelView * vertex;
}
