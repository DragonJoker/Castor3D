#version 140

#define LUMIERE0_ACTIVE 1
#define LUMIERE1_ACTIVE 1
#define LUMIERE2_ACTIVE 1
#define LUMIERE3_ACTIVE 0

#define COEF_LUMIERE0	1.0
#define COEF_LUMIERE1	1.0
#define COEF_LUMIERE2	1.0
#define COEF_LUMIERE3	1.0

#define COEF_AMBIANTE	1.0


#define ACTIVER_OFFSET_MAPPING			1
#define ACTIVER_CORRECTION_OFFSET		1
#define ACTIVER_BUMP_MAPPING			1
#define ACTIVER_OMBRES_INTERNES			1


uniform vec4 		c3d_v4AmbientLight;
uniform vec4 		c3d_v4MatAmbient;
uniform vec4 		c3d_v4MatEmissive;
uniform vec4 		c3d_v4MatDiffuse;
uniform vec4 		c3d_v4MatSpecular;
uniform float 		c3d_fMatShininess;
uniform sampler2D 	c3d_sLights;
uniform int 		c3d_iLightsCount;

uniform sampler2D	c3d_mapDiffuse;
uniform sampler2D	c3d_mapHeight;
uniform sampler2D	c3d_mapNormal;
uniform sampler2D	OffsetMap;
uniform float		HeightMapScale;
uniform float		Scale;
uniform vec3		RVBdiffu;
uniform vec3		RVBspeculaire;

in		vec3		vtx_eyeVec;
in		vec3		PositionVertex;
in		vec3		NormaleVertex;
in		vec2		TexCoord;

out vec4 out_FragColor;

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

vec3 ApplyLight( int p_uiIndex, out float p_fDiffuse, in vec3 NormaleBump )
{
	Light l_light = GetLight( p_uiIndex );
	vec3 vvRVBdiffu0 = l_light.m_v4Diffuse.xyz;//vec3(1.0, 1.0, 0.0);
	vec3 LumiereNormalisee0 = normalize(l_light.m_v4Position.xyz - PositionVertex); 
	p_fDiffuse = max(dot(NormaleBump, LumiereNormalisee0), 0.0);
	vec3 Bump0 =  vec3(COEF_LUMIERE0 * p_fDiffuse) * vvRVBdiffu0.xyz;// + vec3(10.2* pow(diffuse, 16.0)) * vvRVBdiffu.xyz;
	return Bump0;
}

void main()
{
	vec3 CouleurFinale = vec3(0.0);
	vec2 CoordonneesTex0 = TexCoord;
	float l_fDiffuses[10];
	
	#if ACTIVER_OFFSET_MAPPING == 1
	    float hauteur = texture2D( c3d_mapHeight, CoordonneesTex0).r;
    	float v = hauteur * (HeightMapScale * 2.0) - HeightMapScale;
    	vec3 eye = normalize(vtx_eyeVec);
    	vec2 DecalageCoordsTex = (eye.xy * v);

    	#if ACTIVER_CORRECTION_OFFSET == 1
			vec4 MinMax = texture2D(OffsetMap, CoordonneesTex0).rgba;
			float Echelle = Scale; // 0.6 par défaut
	    	DecalageCoordsTex.x = max(-MinMax.z * Echelle, DecalageCoordsTex.x);
	    	DecalageCoordsTex.x = min(MinMax.x * Echelle, DecalageCoordsTex.x);
	    	DecalageCoordsTex.y = max(-MinMax.y * Echelle, DecalageCoordsTex.y);
	    	DecalageCoordsTex.y = min(MinMax.w * Echelle, DecalageCoordsTex.y);
    	#endif
		
//    	DecalageCoordsTex.y = 0.0;
    	CoordonneesTex0 += DecalageCoordsTex;
    #endif

    vec3 RVB = COEF_AMBIANTE * texture2D( c3d_mapDiffuse, CoordonneesTex0).rgb;
	CouleurFinale += RVB;
	
// bump :

	#if ACTIVER_BUMP_MAPPING == 1
		vec3 NormaleBump = texture2D( c3d_mapNormal, CoordonneesTex0).xyz * 2.0 - 1.0;
	#else
		vec3 NormaleBump = vec3(0.0, 0.0, 1.0);
	#endif
	
	vec3 DirectionOeil = normalize(-PositionVertex); // origine - PositionVertex
	
//	vec3 RefletLumiere0 = normalize(-reflect(LumiereNormalisee0, NormaleBump));
//	float speculaire0 = pow(max(dot(DirectionOeil, RefletLumiere0), 0.0), 16.0);

	for( int i = 0 ; i < c3d_iLightsCount && i < 10 ; i++ )
	{
		CouleurFinale += ApplyLight( i, l_fDiffuses[i], NormaleBump );
	}
/*
	#if LUMIERE0_ACTIVE == 1
		vec3 vvRVBdiffu0 = Light0_Diffuse.xyz;//vec3(1.0, 1.0, 0.0);
		vec3 LumiereNormalisee0 = normalize(Light0_Position - PositionVertex); 
		float diffuse0 = max(dot(NormaleBump, LumiereNormalisee0), 0.0);
		vec3 Bump0 =  vec3(COEF_LUMIERE0 * diffuse0) * vvRVBdiffu0.xyz;// + vec3(10.2* pow(diffuse, 16.0)) * vvRVBdiffu.xyz;
		CouleurFinale += Bump0;
	#endif
	#if LUMIERE1_ACTIVE == 1
		vec3 vvRVBdiffu1 = Light1_Diffuse.xyz;//vec3(1.0, 0.0, 0.0);
		vec3 LumiereNormalisee1 = normalize(Light1_Position - PositionVertex); 
		float diffuse1 = max(dot(NormaleBump, LumiereNormalisee1), 0.0);
		vec3 Bump1 =  vec3(COEF_LUMIERE1 * diffuse1) * vvRVBdiffu1.xyz;// + vec3(10.2* pow(diffuse, 16.0)) * vvRVBdiffu.xyz;
		CouleurFinale += Bump1;
	#endif
	#if LUMIERE2_ACTIVE == 1
		vec3 vvRVBdiffu2 = Light2_Diffuse.xyz;//vec3(0.0, 1.0, 0.0);
		vec3 LumiereNormalisee2 = normalize(Light2_Position - PositionVertex); 
		float diffuse2 = max(dot(NormaleBump, LumiereNormalisee2), 0.0);
		vec3 Bump2 =  vec3(COEF_LUMIERE2 * diffuse2) * vvRVBdiffu2.xyz;// + vec3(10.2* pow(diffuse, 16.0)) * vvRVBdiffu.xyz;
		CouleurFinale += Bump2;
	#endif
	#if LUMIERE3_ACTIVE == 1
		vec3 vvRVBdiffu3 = vec3(0.0, 0.0, 1.0);
		vec3 LumiereNormalisee3 = normalize(Light3_Position - PositionVertex); 
		float diffuse3 = max(dot(NormaleBump, LumiereNormalisee3), 0.0);
		vec3 Bump3 =  vec3(COEF_LUMIERE3 * diffuse3) * vvRVBdiffu3.xyz;// + vec3(10.2* pow(diffuse, 16.0)) * vvRVBdiffu.xyz;
		CouleurFinale += Bump3;
	#endif	
*/
	
	for( int i = 0 ; i < c3d_iLightsCount && i < 10 ; i++ )
	{
		float l_fSaturation = clamp( l_fDiffuses[i] * 1.0 + 0.5, 0.0, 1.0 );
		CouleurFinale *= vec3( l_fSaturation );
	}
	
	
/*
	#if LUMIERE0_ACTIVE == 1 && ACTIVER_OMBRES_INTERNES == 1
		float Saturation0 = clamp(diffuse0*1.0 + 0.5, 0.0, 1.0);
		CouleurFinale *= vec3(Saturation0);
	#endif
	#if LUMIERE1_ACTIVE == 1 && ACTIVER_OMBRES_INTERNES == 1
		float Saturation1 = clamp(diffuse1*1.0 + 0.5, 0.0, 1.0);
		CouleurFinale *= vec3(Saturation1);
	#endif
	#if LUMIERE2_ACTIVE == 1 && ACTIVER_OMBRES_INTERNES == 1
		float Saturation2 = clamp(diffuse2*1.0 + 0.5, 0.0, 1.0);
		CouleurFinale *= vec3(Saturation2);
	#endif
	#if LUMIERE3_ACTIVE == 1 && ACTIVER_OMBRES_INTERNES == 1
		float Saturation3 = clamp(diffuse3*1.0 + 0.5, 0.0, 1.0);
		CouleurFinale *= vec3(Saturation3);
	#endif
*/
	out_FragColor = vec4(CouleurFinale.xyz, 1.0);  
//	gl_FragColor = vec4( Light0_Position, 1.0);
}

