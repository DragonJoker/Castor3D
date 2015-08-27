#version 150
#extension GL_EXT_gpu_shader4 : enable
precision highp float;

uniform	sampler2D 	c3d_mapDiffuse;
uniform	sampler2D 	c3d_mapNormal;
uniform	sampler2D	c3d_mapHeight;

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
/*
in		vec2 		vtx_texture;
in		vec3 		vtx_direction;
in		vec3 		vtx_vertex;
in		vec3 		vtx_normal;
in		vec3 		vtx_binormal;
in		vec3 		vtx_tangent;
in		mat4		vtx_mtxModelView;

void main()
{
	float bumpScale = 0.05;
    // We are at height bumpScale.  March forward until we hit a hair or the 
    // base surface.  Instead of dropping down discrete y-voxels we should be
    // marching in texels and dropping our y-value accordingly (TODO: fix)
    float height = 1.0;
	
	Light l_light = GetLight( 0 );
	vec3 tsL;
	vec3 LightDirection = ((vtx_mtxModelView * l_light.m_v4Position).xyz - vtx_vertex);
	tsL.x = dot( vtx_tangent,	LightDirection.xyz	);
	tsL.y = dot( vtx_binormal,	LightDirection.xyz 	);
	tsL.z = dot( vtx_normal,	LightDirection.xyz 	);

    // Number of height divisions
    float numSteps = 5;

    // Texture coordinate marched forward to intersection point
    vec2 offsetCoord = vtx_texture.xy;
    vec4 NB = texture2D( c3d_mapHeight, offsetCoord );

    vec3 tsE = normalize(vtx_direction);

    // Increase steps at oblique angles
    // Note: tsE.z = N dot V
    numSteps = mix(numSteps*2, numSteps, tsE.z);

    // We have to negate tsE because we're walking away from the eye.
    //vec2 delta = vec2(-vtx_direction.x, vtx_direction.y) * bumpScale / (vtx_direction.z * numSteps);
    float step;
    vec2 delta;


    // Constant in z
    step = 1.0 / numSteps;
    delta = vec2(-vtx_direction.x, vtx_direction.y) * bumpScale / (vtx_direction.z * numSteps);

        // Can also step along constant in xy; the results are essentially
        // the same in each case.
        // delta = 1.0 / (25.6 * numSteps) * vec2(-tsE.x, tsE.y);
        // step = tsE.z * bumpScale * (25.6 * numSteps) / (length(tsE.xy) * 400);

    while (NB.a < height) {
        height -= step;
        offsetCoord += delta;
        NB = texture2D( c3d_mapHeight, offsetCoord );
    }

    height = NB.a;

    // Choose the color at the location we hit
    vec3 color = texture2D( c3d_mapDiffuse, offsetCoord ).rgb;

    tsL = normalize(tsL);

    // Use the normals out of the bump map
    vec3 tsN = NB.xyz * 2 - 1;

    // Smooth normals locally along gradient to avoid making slices visible.
    // The magnitude of the step should be a function of the resolution and
    // of the bump scale and number of steps.
//    tsN = normalize(texture2D( c3d_mapHeight, offsetCoord + vec2(tsN.x, -tsN.y) * mipScale).xyz * 2 - 1 + tsN);

    vec3 tsH = normalize(tsL + normalize(vtx_direction));

    float NdotL = max(0, dot(tsN, tsL));
    float NdotH = max(0, dot(tsN, tsH));
    float spec = NdotH * NdotH;

    vec3 lightColor = vec3(1.5, 1.5, 1) * 0.9;
    vec3 ambient = vec3(0.4,0.4,0.6) * 1.4;

    float selfShadow = 0;

    // Don't bother checking for self-shadowing if we're on the
    // back side of an object.
    if (NdotL > 0) {

        // Trace a shadow ray along the light vector.
        int numShadowSteps = int( mix(60,5,tsL.z) );
        step = 1.0 / numShadowSteps;
        delta = vec2(tsL.x, -tsL.y) * bumpScale / (numShadowSteps * tsL.z);

            // We start one iteration out to avoid shadow acne 
            // (could start bumped a little without going
            // a whole iteration).
            height = NB.a + step * 0.1;

            while ((NB.a < height) && (height < 1)) {
                height += step;
                offsetCoord += delta;
                NB = texture2D( c3d_mapHeight, offsetCoord );
            }

            // We are in shadow if we left the loop because
            // we hit a point
            selfShadow = (NB.a < height) ? 1.0 : 0.0;

            // Shadows will make the whole scene darker, so up the light contribution
            lightColor = lightColor * 1.2;
        }

        out_FragColor.xyz = color * ambient
			+ color * NdotL * selfShadow * lightColor;
}
*/
/*
void main()
{
	float 	fHeightMapScale;
	float	fTexelsPerSide;
	float 	fParallaxLimit;
	vec2	vOffset;
	vec3	E;
	vec3	N;
	int 	nNumSamples;
	float	fStepSize;
	vec2 	dx, dy;
	vec2	vOffsetStep;
	vec2	vCurrOffset;
	vec2	vLastOffset;
	vec2	vFinalOffset;
	vec4 	vCurrSample;
	vec4 	vLastSample;
	float 	stepHeight;
	int 	nCurrSample;
	int		nMaxSamples;
	int		nMinSamples;

	nMaxSamples			= 50;
	nMinSamples			= 4;
	fHeightMapScale	= 0.05;
	fTexelsPerSide	= sqrt( 512.0 * 512.0 * 2 );
	fParallaxLimit	= length( vtx_direction.xy ) / vtx_direction.z;							// calculate the parallax offset vector max length
	fParallaxLimit	*= fHeightMapScale;														// scale the vector according to height-map scale
			
	vOffset			= normalize( -vtx_direction.xy );										// calculate the parallax offset vector direction
	vOffset			*= fParallaxLimit;														// calculate the final maximum offset vector
			
	E				= normalize( vtx_direction );			
	N				= normalize( vtx_normal );			
			
	nNumSamples		= int( mix( nMinSamples, nMaxSamples, dot( E, N ) ) );					// calculate dynamic number of samples (Tatarchuk's method)
//	nNumSamples		= int( fParallaxLimit * fTexelsPerSide );								// calculate dynamic number of samples (Zink's method)
	fStepSize		= 1.0 / float( nNumSamples );											// calculate the texcoord step size
			
	dx				= dFdx( vtx_texture );													// calculate the texcoord partial derivative in x in screen space for tex2Dgrad
	dy				= dFdy( vtx_texture );													// calculate the texcoord partial derivative in y in screen space for tex2Dgrad

	vOffsetStep 	= fStepSize * vOffset;
	vCurrOffset 	= vec2( 0, 0 );
	vLastOffset 	= vec2( 0, 0 );
	vFinalOffset 	= vec2( 0, 0 );

	stepHeight		= 1.0;
	nCurrSample		= 0;
	
	vCurrSample		= vec4( 0.0, 0.0, 0.0, 0.0 );
	vLastSample		= vec4( 0.0, 0.0, 0.0, 0.0 );

	while ( nCurrSample < nNumSamples )
	{
		vCurrSample = texture2DGrad( c3d_mapHeight, vtx_texture + vCurrOffset, dx, dy );		// sample the current texcoord offset

		if ( vCurrSample.a > stepHeight )
		{
			float Ua = (vLastSample.a - (stepHeight+fStepSize)) / ( fStepSize + (vCurrSample.a - vLastSample.a));
			vFinalOffset = vLastOffset + Ua * vOffsetStep;
			vCurrSample = texture2DGrad( c3d_mapHeight, vtx_texture + vFinalOffset, dx, dy );	// sample the corrected tex coords
			nCurrSample = nNumSamples + 1;													// exit the while loop
		}
		else
		{
			nCurrSample++;																	// increment to the next sample
			stepHeight -= fStepSize;														// change the required height-map height
			vLastOffset = vCurrOffset;														// remember this texcoord offset for next time
			vCurrOffset += vOffsetStep;														// increment to the next texcoord offset
			vLastSample = vCurrSample;
		}
	}
	
	out_FragColor = vCurrSample;															// return the final sampled color value
}
*/

in		vec4 	position;
in		vec4	textureCoordinates;
in		mat3 	localSurface2World;
in		vec3	viewDirInScaledSurfaceCoords;
in		vec3	_WorldSpaceCameraPos;
in		mat4	vtx_mtxModelViewProjection;

void main()
{
	float _Parallax = 0.01;
	float _MaxTexCoordOffset = 0.01;
	Light l_light = GetLight( 0 );
	
	vec4 _WorldSpaceLightPos0 = vtx_mtxModelViewProjection * l_light.m_v4Position;
	
	// parallax mapping: compute height and 
	// find offset in texture coordinates 
	// for the intersection of the view ray 
	// with the surface at this height

	float height = _Parallax * (-0.5 + texture2D( c3d_mapHeight, textureCoordinates.xy ).x );
	vec2 texCoordOffsets = clamp( height * viewDirInScaledSurfaceCoords.xy / viewDirInScaledSurfaceCoords.z, -_MaxTexCoordOffset, +_MaxTexCoordOffset );

	// normal mapping: lookup and decode normal from bump map

	// in principle we have to normalize the columns 
	// of "localSurface2World" again; however, the potential 
	// problems are small since we use this matrix only 
	// to compute "normalDirection", which we normalize anyways
	vec4 encodedNormal = texture2D( c3d_mapNormal, (textureCoordinates.xy + texCoordOffsets) );
	vec3 localCoords = vec3(2.0 * encodedNormal.ag - vec2(1.0), 0.0);
	localCoords.z = sqrt(1.0 - dot(localCoords, localCoords));
	   // approximation without sqrt: localCoords.z = 
	   // 1.0 - 0.5 * dot(localCoords, localCoords);
	vec3 normalDirection = normalize(localSurface2World * localCoords);

	// per-pixel lighting using the Phong reflection model 
	// (with linear attenuation for point and spot lights)

	vec3 viewDirection = normalize(_WorldSpaceCameraPos - vec3(position));
	vec3 lightDirection;
	float attenuation;

	if (0.0 == _WorldSpaceLightPos0.w) // directional light?
	{
	   attenuation = 1.0; // no attenuation
	   lightDirection = normalize(vec3(_WorldSpaceLightPos0));
	} 
	else // point or spot light
	{
	   vec3 vertexToLightSource = vec3(_WorldSpaceLightPos0 - position);
	   float distance = length(vertexToLightSource);
	   attenuation = 1.0 / distance; // linear attenuation 
	   lightDirection = normalize(vertexToLightSource);
	}

	vec3 ambientLighting = vec3(c3d_v4AmbientLight) * vec3(c3d_v4MatDiffuse);

	vec3 diffuseReflection = attenuation * vec3(l_light.m_v4Diffuse) * vec3(c3d_v4MatDiffuse) * max(0.0, dot(normalDirection, lightDirection));

	vec3 specularReflection;
	if (dot(normalDirection, lightDirection) < 0.0) 
	   // light source on the wrong side?
	{
	   specularReflection = vec3(0.0, 0.0, 0.0); 
		  // no specular reflection
	}
	else // light source on the right side
	{
	   specularReflection = attenuation * vec3(l_light.m_v4Specular) * vec3(c3d_v4MatSpecular) * pow(max(0.0, dot( reflect(-lightDirection, normalDirection), viewDirection)), c3d_fMatShininess);
	}

	out_FragColor = vec4(ambientLighting + diffuseReflection + specularReflection, 1.0);
}
