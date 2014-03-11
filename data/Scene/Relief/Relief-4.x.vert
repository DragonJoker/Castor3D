#version 400

layout( location=0 ) in vec4 vertex;
layout( location=1 ) in vec3 normal;
layout( location=2 ) in vec3 tangent;
layout( location=3 ) in vec3 bitangent;
layout( location=4 ) in vec3 texture;

layout( std140 ) uniform Matrices
{
	mat4 c3d_mtxProjection;
	mat4 c3d_mtxModel;
	mat4 c3d_mtxView;
	mat4 c3d_mtxModelView;
	mat4 c3d_mtxProjectionView;
	mat4 c3d_mtxProjectionModelView;
	mat4 c3d_mtxNormal;
	mat4 c3d_mtxTexture0;
	mat4 c3d_mtxTexture1;
	mat4 c3d_mtxTexture2;
	mat4 c3d_mtxTexture3;
	mat4 c3d_mtxBones[100];
};

out vec3 vtx_vertex;
out vec3 vtx_normal;
out vec3 vtx_tangent;
out vec3 vtx_bitangent;
out vec3 vtx_texture;

void main()
{
	vtx_normal 		= (c3d_mtxNormal * vec4( normal, 0.0 )).xyz;
	vtx_tangent 	= (c3d_mtxNormal * vec4( tangent, 0.0 )).xyz;
	vtx_bitangent 	= cross( vtx_normal, vtx_tangent );
	vtx_texture 	= texture;
	vtx_vertex		= (c3d_mtxModelView * vertex).xyz;

	gl_Position 	= c3d_mtxProjectionModelView * vertex;
}
