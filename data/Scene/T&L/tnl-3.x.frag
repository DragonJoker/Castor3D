#version 140
precision highp float;

in 		vec3 		vtx_normal;
in 		vec3 		vtx_vertex;
in		vec2 		vtx_texture;
in		mat4 		vtx_mtxModelView;

uniform vec4 		c3d_v4AmbientLight;
uniform vec4 		c3d_v4MatAmbient;
uniform vec4 		c3d_v4MatEmissive;
uniform vec4 		c3d_v4MatDiffuse;
uniform vec4 		c3d_v4MatSpecular;
uniform float 		c3d_fMatShininess;
uniform int 		c3d_iLightsCount;
uniform sampler2D 	c3d_sLights;

uniform sampler2D 	c3d_texture;

out		vec4		out_FragColor;

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
	l_lightReturn.m_v4Position		= normalize( vec4( l_v4Position.z, l_v4Position.y, l_v4Position.x, 0.0 ) );
	l_lightReturn.m_iType			= int( l_v4Position.w );
	l_lightReturn.m_mtx4Orientation	= mat4( l_v4A, l_v4B, l_v4C, l_v4D );
	l_lightReturn.m_fExponent		= l_v2Spot.x;
	l_lightReturn.m_fCutOff			= l_v2Spot.x;
	return l_lightReturn;
}

void main()
{
	int 	i;
	vec3 	l_v3Eye;
	vec4 	l_v4Ambient;
	vec4 	l_v4Diffuse;
	vec4 	l_v4Specular;
	vec3 	l_v3Position;
	Light  	l_light;
	float	l_fLambert;
	float	l_fSpecular;
	vec4 	l_v4Texture;

	l_v4Texture		= texture2D( c3d_texture, vtx_texture );
	l_v3Eye			= normalize( -vtx_vertex );
	l_v4Ambient		= c3d_v4MatAmbient * c3d_v4AmbientLight;
	out_FragColor	= vec4( 0.0, 0.0, 0.0, 1.0 );

	for( i = 0 ; i < c3d_iLightsCount ; i++ )
	{
		l_light			= GetLight( i );
		l_v3Position	= normalize( (vtx_mtxModelView * l_light.m_v4Position).xyz );
		l_fLambert		= dot( vtx_normal, l_v3Position );
	
		if( l_fLambert > 0.0 )
		{
			l_fSpecular		= pow( max( dot( reflect( -l_v3Position, vtx_normal ), l_v3Eye ), 0.0 ), c3d_fMatShininess );
			l_v4Diffuse		= c3d_v4MatDiffuse 	* l_fLambert;
			l_v4Specular	= c3d_v4MatSpecular	* l_fSpecular;

			out_FragColor += l_light.m_v4Ambient	* l_v4Ambient;
			out_FragColor += l_light.m_v4Diffuse 	* l_v4Diffuse;
			out_FragColor += l_light.m_v4Specular 	* l_v4Specular;
		}
	}

	out_FragColor = l_v4Texture * out_FragColor;
}
