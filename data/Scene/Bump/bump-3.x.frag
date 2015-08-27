#version 140

uniform		sampler2D	c3d_mapDiffuse;
uniform		sampler2D	c3d_mapNormal;

uniform 	sampler2D 	c3d_sLights;
uniform 	int 		c3d_iLightsCount;
uniform 	vec4 		c3d_v4AmbientLight;
uniform 	vec4 		c3d_v4MatAmbient;
uniform 	vec4 		c3d_v4MatEmissive;
uniform 	vec4 		c3d_v4MatDiffuse;
uniform 	vec4 		c3d_v4MatSpecular;
uniform 	float 		c3d_fMatShininess;

in 			vec3 		vtx_vertex;
in 			vec3 		vtx_normal;
in 			vec3 		vtx_tangent;
in 			vec3 		vtx_binormal;
in			vec2 		vtx_texture;
in		 	vec3		vtx_eyeVec;
in		 	mat4 		vtx_mtxModelView;

out			vec4		out_FragColor;

struct Light
{
	vec4 	m_v4Ambient;
	vec4 	m_v4Diffuse;
	vec4 	m_v4Specular;
	vec4	m_v4Position;
	int 	m_iType;
	vec3	m_v3Attenuation;
	mat4	m_mtx4Orientation;
	float	m_fExponent;
	float	m_fCutOff;
};

Light GetLight( int i )
{
	Light l_lightReturn;
	l_lightReturn.m_v4Ambient		= texture2D( c3d_sLights, vec2( (i * 0.1) + (0 * 0.01) + 0.005, 0.0 ) );
	l_lightReturn.m_v4Diffuse		= texture2D( c3d_sLights, vec2( (i * 0.1) + (1 * 0.01) + 0.005, 0.0 ) );
	l_lightReturn.m_v4Specular		= texture2D( c3d_sLights, vec2( (i * 0.1) + (2 * 0.01) + 0.005, 0.0 ) );
	vec4 	l_v4Position			= texture2D( c3d_sLights, vec2( (i * 0.1) + (3 * 0.01) + 0.005, 0.0 ) );
	l_lightReturn.m_v3Attenuation	= texture2D( c3d_sLights, vec2( (i * 0.1) + (4 * 0.01) + 0.005, 0.0 ) ).xyz;
	vec4 	l_v4A					= texture2D( c3d_sLights, vec2( (i * 0.1) + (5 * 0.01) + 0.005, 0.0 ) );
	vec4 	l_v4B					= texture2D( c3d_sLights, vec2( (i * 0.1) + (6 * 0.01) + 0.005, 0.0 ) );
	vec4 	l_v4C					= texture2D( c3d_sLights, vec2( (i * 0.1) + (7 * 0.01) + 0.005, 0.0 ) );
	vec4 	l_v4D					= texture2D( c3d_sLights, vec2( (i * 0.1) + (8 * 0.01) + 0.005, 0.0 ) );
	vec2	l_v2Spot				= texture2D( c3d_sLights, vec2( (i * 0.1) + (9 * 0.01) + 0.005, 0.0 ) ).xy;
	l_lightReturn.m_v4Position		= vec4( l_v4Position.z, l_v4Position.y, l_v4Position.x, 0.0 );
	l_lightReturn.m_iType			= int( l_v4Position.w );
	l_lightReturn.m_mtx4Orientation	= mat4( l_v4A, l_v4B, l_v4C, l_v4D );
	l_lightReturn.m_fExponent		= l_v2Spot.x;
	l_lightReturn.m_fCutOff			= l_v2Spot.x;
	return l_lightReturn;
}

void main (void)
{
	float	l_fInvRadius;
	Light	l_light;
	vec3	l_v3Tmp;
	vec3 	l_v3Light;
	float 	l_fSqrLength;
	float 	l_fAttenuation;
	vec3 	l_v3EyeVec;
	vec4	l_v4Texture;
	vec3	l_v3Normal;
	vec4	l_v4Ambient;
	vec4	l_v4Diffuse;
	vec4	l_v4Specular;
	
	l_fInvRadius	= 0.02;
	l_light			= GetLight( 0 );
	l_v3Tmp			= normalize( (vtx_mtxModelView * l_light.m_v4Position).xyz );
//	l_v3Tmp			= l_light.m_v4Position.xyz - vtx_vertex;
	l_v3Light.x 	= dot( l_v3Tmp, vtx_tangent		);
	l_v3Light.y 	= dot( l_v3Tmp, vtx_binormal	);
	l_v3Light.z 	= dot( l_v3Tmp, vtx_normal		);
	l_fSqrLength	= dot( l_v3Light, l_v3Light );
	l_v3Light		= l_v3Light * inversesqrt( l_fSqrLength );
	l_fAttenuation	= clamp( 1.0 - l_fInvRadius * sqrt( l_fSqrLength ), 0.0, 1.0 );
	l_v3EyeVec		= normalize( vtx_eyeVec );
	l_v4Texture		= texture2D( c3d_mapDiffuse, vtx_texture );
	l_v3Normal		= normalize( texture2D( c3d_mapNormal, vtx_texture ).xyz * 2.0 - 1.0);
	l_v4Ambient		= l_light.m_v4Ambient	* c3d_v4MatAmbient;
	l_v4Diffuse		= l_light.m_v4Diffuse	* c3d_v4MatDiffuse * max( dot( l_v3Light, l_v3Normal ), 0.0 );
	l_v4Specular	= l_light.m_v4Specular	* c3d_v4MatSpecular * pow( clamp( dot( reflect( -l_v3Light, l_v3Normal ), l_v3EyeVec ), 0.0, 1.0 ), c3d_fMatShininess );	
	
	out_FragColor = (l_v4Ambient * l_v4Texture + l_v4Diffuse * l_v4Texture + l_v4Specular) * l_fAttenuation;
}
