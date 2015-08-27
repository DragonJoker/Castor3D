varying	vec3	tangent;
varying	vec4	vertex;
varying	vec3	normal;
varying	vec2	texture;

uniform mat4 	c3d_mtxProjection;
uniform mat4 	c3d_mtxModel;
uniform mat4 	c3d_mtxView;
uniform mat3 	c3d_mtxNormal;

varying	vec3	vtx_viewVec;
varying	vec3	vtx_eyeVec;
varying	mat4	vtx_mtxModelView;
varying	vec3 	vtx_vertex;
varying	vec3 	vtx_normal;
varying	vec3 	vtx_tangent;
varying	vec3 	vtx_binormal;
varying	vec2	vtx_texture;

void main()
{
	vtx_mtxModelView	= c3d_mtxView * c3d_mtxModel;
	vtx_texture 		= texture.xy;
	vtx_normal 			= normalize( c3d_mtxNormal * normal );
	vtx_tangent 		= normalize( c3d_mtxNormal * tangent );
	vtx_binormal		= cross( vtx_normal, vtx_tangent );
	vtx_vertex			= vec3( vtx_mtxModelView * vertex );

	mat3 TBN_Matrix = mat3( vtx_tangent, vtx_binormal, vtx_normal );
	vec3 tmpVec = -vtx_vertex;
	vtx_eyeVec.x = dot(tmpVec, vtx_tangent	);
	vtx_eyeVec.y = dot(tmpVec, vtx_binormal	);
	vtx_eyeVec.z = dot(tmpVec, vtx_normal	);
	
	vtx_viewVec 		= (-vtx_vertex) * TBN_Matrix;	
	
	gl_Position 		= c3d_mtxProjection * vtx_mtxModelView * vertex;
	
}


