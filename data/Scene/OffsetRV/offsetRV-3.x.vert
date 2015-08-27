#version 140

in		vec3	tangent;
in		vec4	vertex;
in		vec3	normal;
in		vec2	texture;

uniform mat4 	c3d_mtxProjection;
uniform mat4 	c3d_mtxModel;
uniform mat4 	c3d_mtxView;
uniform mat3 	c3d_mtxNormal;

out		mat4	vtx_mtxModelView;
out		vec3 	vtx_eyeVec;
out		vec3	PositionVertex;
out		vec3	NormaleVertex;
out		vec2	TexCoord;

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
