varying vec3 lightVector[3];
varying vec3 eyeVector;
varying vec2 TexCoord;
varying vec4 ex_MatAmbient;
varying vec4 ex_MatDiffuse;

attribute vec3 tangent;
attribute vec4 vertex;
attribute vec3 normal;
attribute vec2 MultiTexCoord0;

uniform vec4 c3d_LightsPosition[8];
uniform vec4 c3d_MatAmbient;
uniform vec4 c3d_MatDiffuse;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;

void main()
{
	gl_Position = ProjectionMatrix * ModelViewMatrix * vertex;
	vec3 cameraInWorldSpace = gl_Position.xyz * ModelViewMatrix[3].xyz;

	TexCoord = MultiTexCoord0;

	vec3 binormal;
	binormal = cross( tangent, normal);  
	binormal = normalize( binormal);

   	mat3 tangentBinormalNormalMatrix = mat3( tangent, binormal, normal);

	vec4 vVertex = ModelViewMatrix * vertex;

	for (int i = 0 ; i < 3 ; i++)
	{
		lightVector[i] = c3d_LightsPosition[i].xyz;
	}
	ex_MatAmbient = c3d_MatAmbient;
	ex_MatDiffuse = c3d_MatDiffuse;

	eyeVector = cameraInWorldSpace - (ModelViewMatrix * vertex).xyz;

}