#version 150

in vec4 vertex;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;

out vec4 vtx_normal;
out vec4 vtx_tangent;
out vec4 vtx_bitangent;

void main()
{
    gl_Position = vertex;
    vtx_normal = vec4(normal, 0.0);
    vtx_tangent = vec4(tangent, 0.0);
    vtx_bitangent = vec4(bitangent, 0.0);
}
