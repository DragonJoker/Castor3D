#version 450
#extension GL_KHR_vulkan_glsl : enable

layout( set=0, binding=0 ) uniform sampler3D mapColour;

layout( location = 0 ) in vec3 vtx_texcoord;

layout( location = 0 ) out vec4 pxl_colour;

void main()
{
	pxl_colour = texture( mapColour, vtx_texcoord );

	if ( pxl_colour.a <= 0.05 )
	{
		discard;
	}
}
