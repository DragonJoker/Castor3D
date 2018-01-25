#version 450
#extension GL_KHR_vulkan_glsl : enable

layout( set=0, binding=0 ) uniform sampler2D mapColour;

layout( location = 0 ) in vec2 vtx_texcoord;

layout( location = 0 ) out vec4 pxl_colour;

void main()
{
	pxl_colour = texture( mapColour, vtx_texcoord );
}
