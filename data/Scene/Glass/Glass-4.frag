#version 150

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

in vec3 vNormal;
in vec3 t;
in vec3 tr;
in vec3 tg;
in vec3 tb;
in float rfac;

out vec4 out_FragColor;

void main()
{
	vec4 ref = c3d_v4BackgroundColour;//textureCube(uCubemap, t);

	vec4 ret = vec4(1);
	ret.r = textureCube(uCubemap, tr).r;
	ret.g = textureCube(uCubemap, tg).g;
	ret.b = textureCube(uCubemap, tb).b;
	
	out_FragColor = ret * rfac + ref * (1.0 - rfac);
}