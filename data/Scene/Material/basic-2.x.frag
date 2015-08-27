varying		vec3 		vtx_normal;
varying		vec3 		vtx_vertex;
varying		vec2 		vtx_texture;
varying	 	mat4 		vtx_mtxModelView;

uniform 	vec4 		c3d_v4AmbientLight;
uniform 	vec4 		c3d_v4MatAmbient;
uniform 	vec4 		c3d_v4MatEmissive;
uniform 	vec4 		c3d_v4MatDiffuse;
uniform 	vec4 		c3d_v4MatSpecular;
uniform 	float 		c3d_fMatShininess;
uniform 	sampler2D 	c3d_sLights;
uniform 	int 		c3d_iLightsCount;

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
	gl_FragColor	= vec4( 0, 0, 0, 0 );
	int 	i;
	vec4 	l_v4Diffuse;
	vec4 	l_v4Specular;
	vec3 	l_v3LightDir;
	Light  	l_light;
	float	l_fLambert;
	float	l_fSpecular;
	vec3	l_v3Eye = normalize( -vtx_vertex );
	vec3	l_v3Normal = normalize( vtx_normal );

	for( i = 0 ; i < c3d_iLightsCount ; i++ )
	{
		l_light			= GetLight( i );
		l_v3LightDir	= normalize( (vtx_mtxModelView * l_light.m_v4Position).xyz );
//		l_v3LightDir	= l_light.m_v4Position.xyz - vtx_vertex;
		l_fLambert		= dot( l_v3Normal, l_v3LightDir );
	
		if( l_fLambert > 0.0 )
		{
			l_fSpecular		= pow( max( dot( reflect( -l_v3LightDir, l_v3Normal ), l_v3Eye ), 0.0 ), c3d_fMatShininess );
			l_v4Diffuse		= c3d_v4MatDiffuse 	* l_fLambert;
			l_v4Specular	= c3d_v4MatSpecular	* l_fSpecular;

			gl_FragColor += l_light.m_v4Ambient 	* c3d_v4MatAmbient;
			gl_FragColor += l_light.m_v4Diffuse 	* l_v4Diffuse;
			gl_FragColor += l_light.m_v4Specular 	* l_v4Specular;
		}
	}
}
