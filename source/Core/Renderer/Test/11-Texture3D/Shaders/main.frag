#version 450
#extension GL_KHR_vulkan_glsl : enable

layout( set=0, binding=0 ) uniform sampler2D mapColour;

layout( location = 0 ) in vec2 vtx_texcoord;

layout( location = 0 ) out vec4 pxl_colour;

void main()
{
#ifdef VULKAN
	pxl_colour = texture( mapColour, vec2( vtx_texcoord.x, vtx_texcoord.y ) );
#else
	pxl_colour = texture( mapColour, vec2( vtx_texcoord.x, 1.0 - vtx_texcoord.y ) );
#endif
}
