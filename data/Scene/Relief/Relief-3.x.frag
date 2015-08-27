#version 140

uniform	float		Depth;
uniform	float		Tile;
uniform	int			LinearSearchSteps;
uniform	int			BinarySearchSteps;
	
in		vec2		vtx_texture;
in		vec3		vtx_vertex;
in		vec3		vtx_tangent;
in		vec3		vtx_binormal;
in		vec3		vtx_normal;
in		mat4		vtx_mtxModelView;

uniform	sampler2D	c3d_mapDiffuse;
uniform	sampler2D	c3d_mapNormal;

uniform vec4 		c3d_v4AmbientLight;
uniform vec4 		c3d_v4MatAmbient;
uniform vec4 		c3d_v4MatEmissive;
uniform vec4 		c3d_v4MatDiffuse;
uniform vec4 		c3d_v4MatSpecular;
uniform float 		c3d_fMatShininess;
uniform sampler2D 	c3d_sLights;
uniform int 		c3d_iLightsCount;

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
	l_lightReturn.m_v4Position		= vec4( l_v4Position.z, l_v4Position.y, l_v4Position.x, 0.0 );
	l_lightReturn.m_iType			= int( l_v4Position.w );
	l_lightReturn.m_mtx4Orientation	= mat4( l_v4A, l_v4B, l_v4C, l_v4D );
	l_lightReturn.m_fExponent		= l_v2Spot.x;
	l_lightReturn.m_fCutOff			= l_v2Spot.x;
	return l_lightReturn;
}

float RayIntersectRM( in vec2 l_v2DP, in vec2 l_v2DS );

vec4 ApplyLight( int p_iIndex, vec4 p_v4Ambient, vec3 p_v3EyeSpaceVertex, float p_fDot, vec2 p_v2DP, vec2 p_v2DS, float p_fD, vec2 p_v2UV, vec3 p_v3Tmp2, vec4 p_v4TextureRelief,  vec4 p_v4TextureColour )
{
	vec4 	l_v4Return;
    float	l_fDot;
    vec2	l_v2DP;
    vec2	l_v2DS;
    vec2	l_v2UV;
	vec3	l_v3Tmp1;
	vec3	l_v3Tmp2;
	vec3	l_v3LightDirection;
	Light	l_light;
	vec4 	l_v4LightSpecular;
	vec4 	l_v4LightDiffuse;
	vec4 	l_v4LightAmbient;
	vec3 	l_v3Light;
	vec4 	l_v4Ambient;
	vec4 	l_v4Specular;
	vec4 	l_v4Diffuse;
	float 	l_fShine;
	float 	l_fDL;
	float 	l_fShadow;
	vec3 	l_v3SpecularShadow;
	float	l_fAttenuation;
	float	l_fDiffuseTerm;
	float	l_fSpecularTerm;

	l_light 			= GetLight( p_iIndex );
	l_v4LightSpecular	= l_light.m_v4Specular	* c3d_v4MatSpecular;
	l_v4LightDiffuse	= l_light.m_v4Diffuse	* c3d_v4MatDiffuse;
	l_v4LightAmbient	= l_light.m_v4Ambient	* c3d_v4MatAmbient;
	l_v3Light			= (vtx_mtxModelView * l_light.m_v4Position).xyz;
	l_v4Ambient			= p_v4Ambient;
	l_v4Specular		= vec4( 0.0 );
	l_v4Diffuse			= vec4( 0.0 );
	
	l_v4Ambient			+= l_v4LightAmbient;
	l_v4Specular 		+= l_v4LightSpecular;
	l_v4Diffuse 		+= l_v4LightDiffuse;

	// compute light direction
	l_v3LightDirection	= normalize( p_v3Tmp2 - l_v3Light.xyz);
   
	// ray intersect in light direction
	l_v2DP				= p_v2DP + (p_v2DS * p_fD);
	l_fDot				= dot( vtx_normal, -l_v3LightDirection );
	l_v3Tmp1			= normalize( vec3( dot( l_v3LightDirection, vtx_tangent ), dot( l_v3LightDirection, vtx_binormal ), l_fDot ) );
	l_v3Tmp1			*= Depth / l_fDot;
	l_v2DS				= l_v3Tmp1.xy;
	l_v2DP				-= l_v2DS * p_fD;
	l_fDL				= RayIntersectRM( l_v2DP, l_v3Tmp1.xy);
	l_fShadow			= 1.0;
	l_v3SpecularShadow	= l_v4Specular.xyz;

	if (l_fDL < p_fD - 0.05) // if pixel in shadow
	{
		l_fShadow			= dot( l_v4Ambient.xyz, vec3( 1.0)) * 0.333333;
		l_v3SpecularShadow	= vec3( 0.0);
	}

	// compute diffuse and specular terms
	l_fAttenuation	= max( 0.0, dot( -l_v3LightDirection, vtx_normal));
	l_fDiffuseTerm	= l_fShadow * max( 0.0, dot( -l_v3LightDirection, p_v4TextureRelief.xyz));
	l_fSpecularTerm	= max( 0.0, dot( normalize( -l_v3LightDirection - p_v3EyeSpaceVertex ), p_v4TextureRelief.xyz ) );

	// compute final color
	l_v4Return.xyz	= (l_v4Ambient.xyz * p_v4TextureColour.xyz / c3d_iLightsCount) + l_fAttenuation * (p_v4TextureColour.xyz * l_v4Diffuse.xyz * l_fDiffuseTerm + l_v3SpecularShadow * pow( l_fSpecularTerm, c3d_fMatShininess));
	l_v4Return.w	= 1.0;
	
	return l_v4Return;
}

void main()
{
    float	l_fDot;
    float	l_fD;
    vec2	l_v2DP;
    vec2	l_v2DS;
    vec2	l_v2UV;
	vec3	l_v3EyeSpaceVertex;
	vec3	l_v3Tmp2;
	vec4	l_v4TextureRelief;
	vec4	l_v4TextureColour;
	vec4	l_v4Ambient;
	
	l_v4Ambient			= c3d_v4MatAmbient * c3d_v4AmbientLight;

	// ray intersect in view direction
	l_v3EyeSpaceVertex	= normalize( vtx_vertex );
	l_fDot				= dot( vtx_normal, -l_v3EyeSpaceVertex );
	l_v2DP				= vtx_texture * Tile;
	l_v2DS				= (normalize( vec3( dot( l_v3EyeSpaceVertex, vtx_tangent), dot( l_v3EyeSpaceVertex, vtx_binormal), l_fDot ) ) * Depth / l_fDot).xy;
	l_fD				= RayIntersectRM( l_v2DP, l_v2DS );
	// get rm and color texture points
	l_v2UV				= l_v2DP + l_v2DS * l_fD;
	l_v4TextureRelief	= vec4( texture2D( c3d_mapNormal, l_v2UV ).rgb, 1.0 );
	l_v4TextureColour	= vec4( texture2D( c3d_mapDiffuse, l_v2UV ).rgb, 1.0 );
	// expand normal from normal map in local polygon space
	l_v4TextureRelief.xy	= l_v4TextureRelief.xy * 2.0 - 1.0;
	l_v4TextureRelief.z		= sqrt( 1.0 - dot( l_v4TextureRelief.xy, l_v4TextureRelief.xy ) );
	l_v4TextureRelief.xyz	= normalize( l_v4TextureRelief.x * vtx_tangent + l_v4TextureRelief.y * vtx_binormal + l_v4TextureRelief.z * vtx_normal );
	l_v3Tmp2				= vtx_vertex + (l_v3EyeSpaceVertex * l_fD * l_fDot);
	
	out_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
	
	for( int i = 0 ; i < c3d_iLightsCount ; i++ )
	{
		out_FragColor += ApplyLight( i, l_v4Ambient, l_v3EyeSpaceVertex, l_fDot, l_v2DP, l_v2DS, l_fD, l_v2UV, l_v3Tmp2, l_v4TextureRelief, l_v4TextureColour );
	}
}

float RayIntersectRM( in vec2 p_vDP, in vec2 p_vDS )
{
	float	l_fDepthStep	= 1.0 / float( LinearSearchSteps);
	// current size of search window
	float	l_fSize			= l_fDepthStep;
	// current depth position
	float	l_fDepth		= 0.0;
	// best match found (starts with last position 1.0)
	float	l_fDepthReturn	= 1.0;
	vec4	l_v4TexColour;

	// search front to back for first point inside object
	for (int i = 0 ; i < LinearSearchSteps - 1 ; i++)
	{
		l_fDepth += l_fSize;
		l_v4TexColour = texture2D( c3d_mapNormal, p_vDP + p_vDS * l_fDepth);

		if (l_fDepthReturn > 0.996)   // if no depth found yet
		{
			if (l_fDepth >= l_v4TexColour.w)
			{
				l_fDepthReturn = l_fDepth;   // store best depth
			}
		}
	}

	l_fDepth = l_fDepthReturn;

	// recurse around first point (l_fDepth) for closest match
	for (int i = 0 ; i < BinarySearchSteps ; i++)
	{
		l_fSize *= 0.5;
		l_v4TexColour = texture2D( c3d_mapNormal, p_vDP + p_vDS * l_fDepth);

		if (l_fDepth >= l_v4TexColour.w)
		{
			l_fDepthReturn = l_fDepth;
			l_fDepth -= 2.0 * l_fSize;
		}

		l_fDepth += l_fSize;
	}

	return l_fDepthReturn;
}
