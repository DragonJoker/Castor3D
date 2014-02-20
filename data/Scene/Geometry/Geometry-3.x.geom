#version 330

precision highp float;

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexData
{
    vec4 color;
    vec3 normal;
} vertexData[];

uniform float explode_factor;

smooth out vec4 color;

void main(void)
{
    vec3 face_normal = normalize (
            cross (gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz,
                gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz));

    for (int i = 0; i < gl_in.length (); ++i)
    {
        color = vertexData[i].color;
        gl_Position = gl_in[i].gl_Position + vec4 (explode_factor * face_normal,
                0.f);
        EmitVertex ();
    }
    EndPrimitive ();
}
