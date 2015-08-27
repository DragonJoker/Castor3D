#version 330

precision highp float;

smooth in vec4 color;

out vec4 outColor;

void main (void)
{
    outColor = color;
}
