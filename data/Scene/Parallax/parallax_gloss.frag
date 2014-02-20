uniform sampler2D	c3d_mapNormal;
uniform sampler2D	c3d_mapDiffuse;

uniform	vec4		c3d_v4AmbientLight;
uniform	vec4		c3d_v4MatAmbient;
uniform	vec4		c3d_v4MatDiffuse;
uniform	vec4		c3d_v4MatEmissive;
uniform	vec4		c3d_v4MatSpecular;
uniform	float		c3d_fMatShininess;
uniform	sampler2D	c3d_sLights;
uniform	vec4		c3d_iLightsCount;

varying	vec3		vtx_viewVec;
varying	mat4		vtx_mtxModelView;
varying	vec3 		vtx_vertex;
varying	vec3 		vtx_normal;
varying	vec3 		vtx_tangent;
varying	vec3 		vtx_binormal;
varying	vec2		vtx_texture;

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

void main()
{
	vec2	cBumpSize			= 0.02 * vec2 (2.0, -1.0);
	mat3	TBN_Matrix	 		= mat3( vtx_tangent, vtx_binormal, vtx_normal );
	Light	l_light 			= GetLight( 0 );
	vec4	g_lightEye			= vtx_mtxModelView * l_light.m_v4Position;
	vec3	g_lightVec			= 0.02 * (g_lightEye.xyz - vtx_vertex) * TBN_Matrix;
	float	LightAttenuation	= clamp(1.0 - dot( g_lightVec, g_lightVec ), 0.0, 1.0 );
	vec3 	lightVec			= normalize( g_lightVec );
	vec3 	viewVec				= normalize( vtx_viewVec );
	vec4	l_texTmp			= texture2D( c3d_mapNormal, vtx_texture );
	float	height				= l_texTmp.a * cBumpSize.x + cBumpSize.y;
	vec2	newUV				= vtx_texture + viewVec.xy * height;
	vec4	color_base			= texture2D( c3d_mapDiffuse, newUV );
	l_texTmp					= texture2D( c3d_mapNormal, newUV );
	vec3	bump				= normalize( l_texTmp.rgb * 2.0 - 1.0 );
	l_texTmp					= texture2D( c3d_mapNormal, newUV );
	float 	base				= l_texTmp.a;	
	float 	diffuse				= clamp( dot( lightVec, bump ), 0.0, 1.0 );
	float 	specular			= pow( clamp( dot( reflect( -viewVec, bump ), lightVec ), 0.0, 1.0 ), 16.0 );

	gl_FragColor = vec4(color_base.rgb * l_light.m_v4Diffuse.rgb * (diffuse * base + 0.7 * specular * color_base.a) * LightAttenuation, 1.0 );
}

