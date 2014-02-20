#version 140

uniform vec4 c3d_MatDiffuse;
uniform vec4 c3d_MatAmbient;
uniform float c3d_MatShininess;
uniform vec4 c3d_MatSpecular;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform vec4 c3d_LightsPosition[8];
uniform vec4 c3d_LightsDiffuse[8];
uniform vec4 c3d_LightsAmbient[8];
uniform vec4 c3d_LightsSpecular[8];

in vec3 tangent;
in vec4 vertex;
in vec3 normal;
in vec2 texture;

out vec3 eyeNormal;
out vec3 eyeVertex;
out vec2 texCoord;

out vec3 lightDir[8];
out vec4 lightDiff[8];
out vec4 lightAmbt[8];
out vec4 lightSpec[8];

out vec4 ex_MatDiffuse;
out vec4 ex_MatAmbient;
out vec4 ex_MatSpecular;
out float ex_MatShininess;

void main()
{
	eyeNormal = NormalMatrix * normal;
	eyeVertex = (ModelViewMatrix * vertex).xyz;

	for (int i = 0 ; i < 3 ; ++i)
	{
		lightDir[i] = (ModelViewMatrix * c3d_LightsPosition[i]).xyz;
		lightDiff[i] = c3d_LightsDiffuse[i];
		lightSpec[i] = c3d_LightsSpecular[i];
		lightAmbt[i] = c3d_LightsAmbient[i];
	}

	ex_MatDiffuse = c3d_MatDiffuse;
	ex_MatShininess = c3d_MatShininess;
	ex_MatSpecular = c3d_MatSpecular;
	ex_MatAmbient = c3d_MatAmbient;

	texCoord = MultiTexCoord0;

	gl_Position = ProjectionMatrix * ModelViewMatrix * vertex;
}