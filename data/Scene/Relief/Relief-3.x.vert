#version 140

in		vec3	tangent;
in		vec4	vertex;
in		vec3	normal;
in		vec2	texture;

uniform mat4 	c3d_mtxProjection;
uniform mat4 	c3d_mtxModel;
uniform mat4 	c3d_mtxView;
uniform mat4 	c3d_mtxModelView;
uniform mat3 	c3d_mtxNormal;

out		vec2	vtx_texture;
out		vec3	vtx_vertex;
out		vec3	vtx_tangent;
out		vec3	vtx_binormal;
out		vec3	vtx_normal;
out		mat4	vtx_mtxModelView;

void main()
{
	vtx_mtxModelView 	= c3d_mtxModelView;
	vtx_normal 			= c3d_mtxNormal * normal;
	vtx_tangent 		= c3d_mtxNormal * tangent;
	vtx_binormal 		= cross( vtx_normal, vtx_tangent );
	vtx_texture 		= texture.xy;
	vtx_vertex			= (vtx_mtxModelView * vertex).xyz;

	gl_Position 		= c3d_mtxProjection * vtx_mtxModelView * vertex;
}
