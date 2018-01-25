#version 450
#extension GL_KHR_vulkan_glsl : enable

layout( set=0, binding=0 ) uniform samplerBuffer mapColour;

layout( location = 0 ) in float vtx_texcoord;

layout( location = 0 ) out vec4 pxl_colour;

void main()
{
	pxl_colour = texelFetch( mapColour, int( clamp( vtx_texcoord * 512, 0.0, 511.0 ) ) );
}
