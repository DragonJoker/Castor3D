#version 150
uniform	sampler2D 	c3d_sLights;
layout( std140 ) uniform Scene
{
	int		c3d_iLightsCount;
	vec4	c3d_v4AmbientLight;
	vec4	c3d_v4BackgroundColour;
	vec3	c3d_v3CameraPosition;
};
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
layout( std140 ) uniform PassColours
{
	vec4	c3d_v4MatAmbient;
	vec4	c3d_v4MatDiffuse;
	vec4	c3d_v4MatEmissive;
	vec4	c3d_v4MatSpecular;
	float	c3d_fMatShininess;
	float	c3d_fMatOpacity;
};
in Data
{
	vec3	m_v3Vertex;
	vec3	m_v3Normal;
	vec3	m_v3Tangent;
	vec3	m_v3Bitangent;
	vec2	m_v2Texture;
	vec3	m_v3Eye;
} vtx_out;
out	vec4	out_FragColor;
struct Light
{
	vec4	m_v4Ambient;
	vec4	m_v4Diffuse;
	vec4	m_v4Specular;
	vec4	m_v4Position;
	int		m_iType;
	vec3	m_v3Attenuation;
	mat4	m_mtx4Orientation;
	float	m_fExponent;
	float	m_fCutOff;
};
Light GetLight( int i )
{
	Light	l_lightReturn;
	l_lightReturn.m_v4Ambient		= texture2D( c3d_sLights, vec2( (i * 0.1) + (0 * 0.01) + 0.005, 0.0 ) );
	l_lightReturn.m_v4Diffuse		= texture2D( c3d_sLights, vec2( (i * 0.1) + (1 * 0.01) + 0.005, 0.0 ) );
	l_lightReturn.m_v4Specular		= texture2D( c3d_sLights, vec2( (i * 0.1) + (2 * 0.01) + 0.005, 0.0 ) );
	vec4	l_v4Position			= texture2D( c3d_sLights, vec2( (i * 0.1) + (3 * 0.01) + 0.005, 0.0 ) );
	l_lightReturn.m_v3Attenuation	= texture2D( c3d_sLights, vec2( (i * 0.1) + (4 * 0.01) + 0.005, 0.0 ) ).xyz;
	vec4	l_v4A					= texture2D( c3d_sLights, vec2( (i * 0.1) + (5 * 0.01) + 0.005, 0.0 ) );
	vec4	l_v4B					= texture2D( c3d_sLights, vec2( (i * 0.1) + (6 * 0.01) + 0.005, 0.0 ) );
	vec4	l_v4C					= texture2D( c3d_sLights, vec2( (i * 0.1) + (7 * 0.01) + 0.005, 0.0 ) );
	vec4	l_v4D					= texture2D( c3d_sLights, vec2( (i * 0.1) + (8 * 0.01) + 0.005, 0.0 ) );
	vec2	l_v2Spot				= texture2D( c3d_sLights, vec2( (i * 0.1) + (9 * 0.01) + 0.005, 0.0 ) ).xy;
	l_lightReturn.m_v4Position		= vec4( l_v4Position.z, l_v4Position.y, l_v4Position.x, 0.0 );
	l_lightReturn.m_iType			= int( l_v4Position.w );
	l_lightReturn.m_mtx4Orientation	= mat4( l_v4A, l_v4B, l_v4C, l_v4D );
	l_lightReturn.m_fExponent		= l_v2Spot.x;
	l_lightReturn.m_fCutOff			= l_v2Spot.x;
	return l_lightReturn;
}
void main()
{
	out_FragColor	= vec4( 0, 0, 0, 0 );
	Light	l_light;
	vec3	l_v3LightDir;
	float	l_fLambert;
	float	l_fAttenuation;
	float	l_fSpecular;
	vec3	l_v3Normal			= normalize( vtx_out.m_v3Normal );
	vec4	l_v4Ambient			= vec4( 0, 0, 0, 0 );
	vec4	l_v4Diffuse			= vec4( 0, 0, 0, 0 );
	vec4	l_v4Specular		= vec4( 0, 0, 0, 0 );
	vec4	l_v4TmpAmbient		= vec4( 0, 0, 0, 0 );
	vec4	l_v4TmpDiffuse		= vec4( 0, 0, 0, 0 );
	vec4	l_v4TmpSpecular		= vec4( 0, 0, 0, 0 );
	vec3	l_v3EyeVec			= normalize( vtx_out.m_v3Eye );
	float	l_fAlpha			= c3d_fMatOpacity;
	vec4	l_v4MatSpecular		= c3d_v4MatSpecular;
	vec3	l_v3LightPosition;
	for( int i = 0 ; i < c3d_iLightsCount ; i++ )
	{
		l_light = GetLight( i );
		l_v4MatSpecular = c3d_v4MatSpecular;
		l_v3LightPosition = l_light.m_v4Position.xyz;
		if( 0 == l_light.m_iType ) // directional light?
		{
			l_v3LightDir= normalize( l_v3LightPosition );
			l_fAttenuation = 1.0;
		}
		else // point or spot light
		{
			vec3 l_v3VtxToLight = vec3( l_v3LightPosition - vtx_out.m_v3Vertex );
			float l_fDistance = length( l_v3VtxToLight );
			l_fAttenuation = 1.0 / (l_light.m_v3Attenuation.x + l_fDistance * (l_light.m_v3Attenuation.y + l_fDistance * l_light.m_v3Attenuation.z));
			l_v3LightDir = normalize( l_v3VtxToLight );
		}
		l_fLambert = dot( l_v3Normal, l_v3LightDir );
		if( l_fLambert < 0.0 ) // light source on the wrong side?
		{
			l_fSpecular = 0.0; // no specular reflection
			l_fLambert = 0.0;
		}
		else // light source on the right side
		{
			l_fSpecular	= pow( clamp( dot( reflect( -l_v3LightDir, l_v3Normal ), l_v3EyeVec ), 0.0, 1.0 ), c3d_fMatShininess );
			float l_fFresnel = pow( 1.0 - max( 0.0, dot( normalize( l_v3LightDir + l_v3EyeVec ), l_v3EyeVec ) ), 5.0 );
			l_v4MatSpecular = vec4( mix( vec3( c3d_v4MatSpecular ), vec3( 1.0 ), l_fFresnel ), 1.0 );
		}
		l_v4TmpAmbient	= c3d_v4AmbientLight * l_light.m_v4Ambient * c3d_v4MatAmbient;
		l_v4TmpDiffuse	= l_fAttenuation * l_light.m_v4Diffuse * c3d_v4MatDiffuse * l_fLambert;
		l_v4TmpSpecular	= l_fAttenuation * l_light.m_v4Specular * l_v4MatSpecular * l_fSpecular;
		l_v4Ambient		+= l_v4TmpAmbient;
		l_v4Diffuse		+= l_v4TmpDiffuse;
		l_v4Specular	+= l_v4TmpSpecular;
	}
	out_FragColor = vec4( (l_v4Ambient + l_v4Diffuse + l_v4Specular).xyz, l_fAlpha );
}