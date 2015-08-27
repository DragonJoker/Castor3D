#version 150
in	vec4	vertex;
in	vec3	normal;
in	vec3	tangent;
in	vec3	bitangent;
in	vec2	texture;
layout( std140 ) uniform Matrices
{
	mat4	c3d_mtxProjection;
	mat4	c3d_mtxModel;
	mat4	c3d_mtxView;
	mat4	c3d_mtxModelView;
	mat4	c3d_mtxProjectionModelView;
	mat4	c3d_mtxNormal;
	mat4	c3d_mtxTexture0;
	mat4	c3d_mtxTexture1;
	mat4	c3d_mtxTexture2;
	mat4	c3d_mtxTexture3;
};
out Data
{
	vec3	m_v3Vertex;
	vec3	m_v3Normal;
	vec3	m_v3Tangent;
	vec3	m_v3Bitangent;
	vec2	m_v2Texture;
	vec3	m_v3Eye;
} vtx_out;
void main(void)
{
	mat3 l_mtxNormal = mat3( c3d_mtxNormal );
	vtx_out.m_v2Texture 	= texture.xy;
	vtx_out.m_v3Normal 		= normalize( l_mtxNormal * normal );
	vtx_out.m_v3Tangent 	= normalize( l_mtxNormal * tangent );
	vtx_out.m_v3Bitangent	= normalize( l_mtxNormal * bitangent );
	vtx_out.m_v3Vertex		= vec3( c3d_mtxModelView * vertex );
	vtx_out.m_v3Eye			= -vtx_out.m_v3Vertex;
	gl_Position 			= c3d_mtxProjectionModelView * vertex;
}