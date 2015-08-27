attribute	vec3	tangent;
attribute	vec4	vertex;
attribute	vec3	normal;
attribute	vec2	texture;

uniform		mat4 	c3d_mtxProjection;
uniform		mat4 	c3d_mtxModel;
uniform		mat4 	c3d_mtxView;
uniform		mat4 	c3d_mtxModelView;
uniform		mat3 	c3d_mtxNormal;

varying		vec2	vtx_texture;
varying		vec3	vtx_vertex;
varying		vec3	vtx_tangent;
varying		vec3	vtx_binormal;
varying		vec3	vtx_normal;
varying		mat4	vtx_mtxModelView;

void main()
{
	vtx_mtxModelView 	= c3d_mtxModelView;
	vtx_tangent 		= c3d_mtxNormal * tangent;
	vtx_normal 			= c3d_mtxNormal * normal;
	vtx_binormal 		= cross( vtx_tangent, vtx_normal );
	vtx_texture 		= texture.xy;
	vtx_vertex			= (vtx_mtxModelView * vertex).xyz;

	gl_Position 		= c3d_mtxProjection * vtx_mtxModelView * vertex;
}
