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

attribute vec3 tangent;
attribute vec4 vertex;
attribute vec3 normal;
attribute vec2 texture;

varying vec3 eyeNormal;
varying vec3 eyeVertex;
varying vec2 texCoord;

varying vec3 lightDir[8];
varying vec4 lightDiff[8];
varying vec4 lightAmbt[8];
varying vec4 lightSpec[8];

varying vec4 ex_MatDiffuse;
varying vec4 ex_MatAmbient;
varying vec4 ex_MatSpecular;
varying float ex_MatShininess;

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
