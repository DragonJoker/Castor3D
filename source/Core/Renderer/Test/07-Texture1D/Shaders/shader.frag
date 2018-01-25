#version 450
#extension GL_KHR_vulkan_glsl : enable

layout( set=0, binding=0 ) uniform sampler1D mapColour;

layout( location = 0 ) in float vtx_texcoord;

layout( location = 0 ) out vec4 pxl_colour;

void main()
{
	pxl_colour = texture( mapColour, vtx_texcoord );
}
