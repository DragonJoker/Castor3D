#version 330

precision highp float;

// Attributes per vertex: position and normal
in vec4 vertex;
in vec3 normal;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform vec4 c3d_LightsPosition[8];

out VertexData
{
	vec4 color;
	vec3 normal;
} vertexData;

void main()
{
    vec3 N = normalize( NormalMatrix * normal);

    // Get vertex position in eye coordinates
    vec4 vertexPos = ModelViewMatrix * vertex;
    vec3 vertexEyePos = vertexPos.xyz / vertexPos.w;

    // Get vector to light source
    vec3 L = normalize(c3d_LightsPosition[0].xyz - vertexEyePos);

    // Dot product gives us diffuse intensity
    vertexData.color = vec4 (.3f, .3f, .9f, 1.f) * max (0.f, dot (N, L));

    // Don't forget to transform the geometry!
    gl_Position = ProjectionMatrix * ModelViewMatrix * vertex;
    vertexData.normal = normal;
}
