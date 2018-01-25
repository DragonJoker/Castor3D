#version 450
#extension GL_KHR_vulkan_glsl : enable

layout( set=0, binding=0 ) uniform samplerCube mapColour;

layout( location = 0 ) in vec3 vtx_texcoord;

layout( location = 0 ) out vec4 pxl_colour;

void main()
{
	pxl_colour = texture( mapColour, vec3( vtx_texcoord.x, -vtx_texcoord.y, vtx_texcoord.z ) );
}
