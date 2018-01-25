#version 450

layout( location = 0 ) in vec4 vtx_colour;

layout( location = 0 ) out vec4 pxl_colour;

void main()
{
  pxl_colour = vtx_colour;
}
