varying	vec4 	vertex;
varying	vec3 	normal;
varying	vec3 	tangent;
varying	vec2 	texture;

uniform mat4 	c3d_mtxProjection;
uniform mat4 	c3d_mtxModel;
uniform mat4 	c3d_mtxView;
uniform mat3 	c3d_mtxNormal;

varying	vec2 	vtx_texture;
varying	vec3 	vtx_vertex;
varying	vec3 	vtx_tangent;
varying	vec3 	vtx_binormal;
varying	vec3 	vtx_normal;
varying	mat4 	vtx_mtxModelView;
varying	vec3	vtx_eyeVec;

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
