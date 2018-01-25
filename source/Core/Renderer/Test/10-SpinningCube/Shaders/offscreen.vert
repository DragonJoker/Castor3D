#version 450
#extension GL_KHR_vulkan_glsl : enable

layout( set=0, binding=1 ) uniform Matrix
{
	mat4 mtxProjection;
};

layout( set=0, binding=2 ) uniform Object
{
	mat4 mtxModel;
};

layout( location=0 ) in vec4 position;
layout( location=1 ) in vec2 texcoord;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout( location = 0 ) out vec2 vtx_texcoord;

void main()
{
	gl_Position = mtxProjection * mtxModel * position;
	vtx_texcoord = texcoord;
}
