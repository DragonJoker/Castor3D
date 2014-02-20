#version 140
precision highp float;

in		vec4 	vertex;
in		vec3 	normal;
in		vec3 	tangent;
in		vec2 	texture;

uniform mat4 	c3d_mtxProjection;
uniform mat4 	c3d_mtxModel;
uniform mat4 	c3d_mtxView;
uniform mat3 	c3d_mtxNormal;

out		vec2 	vtx_texture;
out		vec3 	vtx_vertex;
out		vec3 	vtx_tangent;
out		vec3 	vtx_binormal;
out		vec3 	vtx_normal;
out		mat4 	vtx_mtxModelView;
out		vec3	vtx_eyeVec;

void main()
{
	vtx_mtxModelView	= c3d_mtxView * c3d_mtxModel;
	mat3 TBN_Matrix;
	vec3 binormal = cross( tangent, normal );
	
	vtx_tangent			= normalize( c3d_mtxNormal * tangent );
	vtx_binormal		= normalize( c3d_mtxNormal * binormal );
	vtx_normal			= normalize( c3d_mtxNormal * normal );
	
	TBN_Matrix[0] =  vtx_tangent;
	TBN_Matrix[1] =  vtx_binormal;
	TBN_Matrix[2] =  vtx_normal;
	
	vtx_vertex 			= (vtx_mtxModelView * vertex).xyz;
	vtx_texture			= texture.xy;
	vtx_eyeVec			= -vtx_vertex * TBN_Matrix;

	gl_Position = c3d_mtxProjection * vtx_mtxModelView * vertex;
}
