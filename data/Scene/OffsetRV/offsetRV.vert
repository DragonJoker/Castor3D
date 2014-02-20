attribute	vec3	tangent;
attribute	vec4	vertex;
attribute	vec3	normal;
attribute	vec2	texture;

uniform		mat4 	c3d_mtxProjection;
uniform		mat4 	c3d_mtxModel;
uniform		mat4 	c3d_mtxView;
uniform		mat3 	c3d_mtxNormal;

varying		mat4	vtx_mtxModelView;
varying		vec3 	vtx_eyeVec;
varying		vec3	PositionVertex;
varying		vec3	NormaleVertex;
varying		vec2	TexCoord;

void main() 
{ 
	TexCoord = texture;

	vec3 binormal = cross( tangent, normal);

	mat3 TBN_Matrix;
	TBN_Matrix[0] =  c3d_mtxNormal * tangent;
	TBN_Matrix[1] =  c3d_mtxNormal * binormal;
	TBN_Matrix[2] =  c3d_mtxNormal * normal;

	// transforme le vecteur vision :
	vtx_mtxModelView	= c3d_mtxView * c3d_mtxModel;
	vec4 VertexModelView = vtx_mtxModelView * vertex; 
	vtx_eyeVec = vec3( -VertexModelView ) * TBN_Matrix;

	PositionVertex = vec3( VertexModelView ) * TBN_Matrix;
	NormaleVertex = normalize( c3d_mtxNormal * normal);

	// Vertex transformation 
	gl_Position = c3d_mtxProjection * vtx_mtxModelView * vertex;
}
