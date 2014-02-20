#version 150

in vec4 vertex;
in vec3 normal;

layout( std140 ) uniform Matrices
{
	mat4	c3d_mtxProjection;
	mat4	c3d_mtxModel;
	mat4	c3d_mtxView;
	mat4	c3d_mtxModelView;
	mat4	c3d_mtxProjectionModelView;
	mat4	c3d_mtxNormal;
	mat4	c3d_mtxTexture0;
	mat4	c3d_mtxTexture1;
	mat4	c3d_mtxTexture2;
	mat4	c3d_mtxTexture3;
};
layout( std140 ) uniform Scene
{
	int 	c3d_iLightsCount;
	vec4 	c3d_v4AmbientLight;
	vec4 	c3d_v4BackgroundColour;
	vec3 	c3d_v3CameraPos;
};

out vec3 vNormal;
out vec3 t;
out vec3 tr;
out vec3 tg;
out vec3 tb;
out float rfac;

uniform vec3 chromaticDispertion;
uniform float bias;
uniform float scale;
uniform float power;

void main()
 {
	gl_Position = c3d_mtxProjectionModelView * vertex;
	vNormal = normalize(c3d_mtxNormal * normal);	
	vec3 incident = normalize( (vertex * c3d_mtxModel).xyz - c3d_v3CameraPos);
	
	t = reflect(incident, vNormal);	
	tr = refract(incident, vNormal, chromaticDispertion.x);
	tg = refract(incident, vNormal, chromaticDispertion.y);
	tb = refract(incident, vNormal, chromaticDispertion.z);
	
	// bias, scale, 1, power
	rfac = bias + scale * pow(1.0 + dot(incident, vNormal), power);
}