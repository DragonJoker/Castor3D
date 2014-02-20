#version 420 core

in	vec4 	vertex;
in	vec3	normal;
in	vec3	tangent;
in	vec3	bitangent;
in	vec2	texture;
out	vec3	vtx_vertex;
out	vec3	vtx_normal;
out	vec3	vtx_tangent;
out	vec3	vtx_bitangent;
out	vec2	vtx_texture;

void main()
{
	vtx_vertex		= vertex.xyz;
	vtx_normal		= normal;
	vtx_tangent		= tangent;
	vtx_bitangent	= bitangent;
	vtx_texture		= texture;
}
