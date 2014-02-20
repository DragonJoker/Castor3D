attribute	vec4 	vertex;
attribute	vec3 	normal;
attribute	vec2 	texture;

uniform		mat4 	c3d_mtxProjectionModelView;
uniform		mat4 	c3d_mtxModelView;
uniform		mat3 	c3d_mtxNormal;

varying		vec2 	vtx_texture;
varying 	vec3 	vtx_normal;
varying 	vec3 	vtx_vertex;
varying 	mat4 	vtx_mtxModelView;

void main()
{
	vtx_mtxModelView 	= c3d_mtxModelView;
	vtx_normal 			= c3d_mtxNormal * normal;
	vtx_vertex 			= (vtx_mtxModelView * vertex).xyz;
	vtx_texture			= texture.xy;

	gl_Position = c3d_mtxProjectionModelView * vertex;
}
