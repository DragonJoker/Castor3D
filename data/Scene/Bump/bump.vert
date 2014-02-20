attribute	vec4 	vertex;
attribute	vec3 	normal;
attribute	vec3	tangent;
attribute	vec2 	texture;

uniform		mat4	c3d_mtxProjection;
uniform		mat4	c3d_mtxModel;
uniform		mat4	c3d_mtxView;
uniform		mat3	c3d_mtxNormal;

varying		vec3 	vtx_vertex;
varying		vec3 	vtx_normal;
varying		vec3 	vtx_tangent;
varying		vec3 	vtx_binormal;
varying		vec2 	vtx_texture;
varying		vec3	vtx_eyeVec;
varying	 	mat4	vtx_mtxModelView;

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
