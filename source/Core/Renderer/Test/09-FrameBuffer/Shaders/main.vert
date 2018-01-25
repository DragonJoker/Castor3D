#version 450

layout( location = 0 ) in vec4 position;
layout( location = 1 ) in vec2 texcoord;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout( location = 0 ) out vec2 vtx_texcoord;

void main()
{
    gl_Position = position;
    vtx_texcoord = texcoord;
}
