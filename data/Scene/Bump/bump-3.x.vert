#version 140

in 		vec4 	vertex;
in 		vec3 	normal;
in		vec3	tangent;
in 		vec2 	texture;

uniform	mat4	c3d_mtxProjection;
uniform	mat4	c3d_mtxModel;
uniform	mat4	c3d_mtxView;
uniform	mat3	c3d_mtxNormal;

out		vec3 	vtx_vertex;
out		vec3 	vtx_normal;
out		vec3 	vtx_tangent;
out		vec3 	vtx_binormal;
out		vec2 	vtx_texture;
out		vec3	vtx_eyeVec;
out	 	mat4	vtx_mtxModelView;

void main(void)
{
	vtx_mtxModelView	= c3d_mtxView * c3d_mtxModel;
	vtx_texture 		= texture.xy;
	vtx_normal 			= normalize( c3d_mtxNormal * normal );
	vtx_tangent 		= normalize( c3d_mtxNormal * tangent );
	vtx_binormal		= cross( vtx_tangent, vtx_normal );
	vtx_vertex			= vec3( vtx_mtxModelView * vertex );

	vec3 tmpVec = -vtx_vertex;
	vtx_eyeVec.x = dot( tmpVec, vtx_tangent		);
	vtx_eyeVec.y = dot( tmpVec, vtx_binormal	);
	vtx_eyeVec.z = dot( tmpVec, vtx_normal		);
	
	gl_Position 		= c3d_mtxProjection * vtx_mtxModelView * vertex;
}
