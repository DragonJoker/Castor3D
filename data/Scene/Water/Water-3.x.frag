#version 140

layout( std140 ) uniform Matrices
{
    mat4 c3d_mtxProjection;
    mat4 c3d_mtxModel;
    mat4 c3d_mtxView;
    mat4 c3d_mtxModelView;
    mat4 c3d_mtxProjectionView;
    mat4 c3d_mtxProjectionModelView;
    mat4 c3d_mtxNormal;
    mat4 c3d_mtxTexture0;
    mat4 c3d_mtxTexture1;
    mat4 c3d_mtxTexture2;
    mat4 c3d_mtxTexture3;
    mat4 c3d_mtxBones[100];
};

layout( std140 ) uniform Scene
{
    int c3d_iLightsCount;
    vec4 c3d_v4AmbientLight;
    vec4 c3d_v4BackgroundColour;
    vec3 c3d_v3CameraPosition;
};

layout( std140 ) uniform PassColours
{
    vec4 c3d_v4MatAmbient;
    vec4 c3d_v4MatDiffuse;
    vec4 c3d_v4MatEmissive;
    vec4 c3d_v4MatSpecular;
    float c3d_fMatShininess;
    float c3d_fMatOpacity;
};

uniform sampler1D c3d_sLights;

uniform float c3d_fTime;
uniform vec3 color;

uniform vec3 c3d_v3MatDiffuse;

uniform sampler2D normalSampler;
uniform sampler2D reflectionSampler;
uniform sampler2D refractionSampler;

in vec3 worldPosition;
in vec4 projectedPosition;

out vec4 out_FragColor;

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

Light GetLight( int p_iIndex )
{
	Light	l_lightReturn;
	float l_fFactor = (float( p_iIndex ) * 0.01);
	l_lightReturn.m_v4Ambient		= texture( c3d_sLights, l_fFactor + (0 * 0.001) + 0.0005 );
	l_lightReturn.m_v4Diffuse		= texture( c3d_sLights, l_fFactor + (1 * 0.001) + 0.0005 );
	l_lightReturn.m_v4Specular		= texture( c3d_sLights, l_fFactor + (2 * 0.001) + 0.0005 );
	vec4	l_v4Position			= texture( c3d_sLights, l_fFactor + (3 * 0.001) + 0.0005 );
	l_lightReturn.m_v3Attenuation	= texture( c3d_sLights, l_fFactor + (4 * 0.001) + 0.0005 ).xyz;
	vec4	l_v4A					= texture( c3d_sLights, l_fFactor + (5 * 0.001) + 0.0005 );
	vec4	l_v4B					= texture( c3d_sLights, l_fFactor + (6 * 0.001) + 0.0005 );
	vec4	l_v4C					= texture( c3d_sLights, l_fFactor + (7 * 0.001) + 0.0005 );
	vec4	l_v4D					= texture( c3d_sLights, l_fFactor + (8 * 0.001) + 0.0005 );
	vec2	l_v2Spot				= texture( c3d_sLights, l_fFactor + (9 * 0.001) + 0.0005 ).xy;
	l_lightReturn.m_v4Position		= vec4( l_v4Position.z, l_v4Position.y, l_v4Position.x, 0.0 );
	l_lightReturn.m_iType			= int( l_v4Position.w );
	l_lightReturn.m_mtx4Orientation	= mat4( l_v4A, l_v4B, l_v4C, l_v4D );
	l_lightReturn.m_fExponent		= l_v2Spot.x;
	l_lightReturn.m_fCutOff			= l_v2Spot.x;
	return l_lightReturn;
}

vec4 getNoise(vec2 uv)
{
    vec2 uv0 = (uv/103.0)+vec2(c3d_fTime/17.0, c3d_fTime/29.0);
    vec2 uv1 = uv/107.0-vec2(c3d_fTime/-19.0, c3d_fTime/31.0);
    vec2 uv2 = uv/vec2(897.0, 983.0)+vec2(c3d_fTime/101.0, c3d_fTime/97.0);
    vec2 uv3 = uv/vec2(991.0, 877.0)-vec2(c3d_fTime/109.0, c3d_fTime/-113.0);
    vec4 noise = (texture2D(normalSampler, uv0)) +
                 (texture2D(normalSampler, uv1)) +
                 (texture2D(normalSampler, uv2)) +
                 (texture2D(normalSampler, uv3));
    return noise*0.5-1.0;
}

void sunLight(const vec3 surfaceNormal, const vec3 eyeDirection, float shiny, float spec, float diffuse, inout vec3 diffuseColor, inout vec3 specularColor)
{
    Light l_light = GetLight( 0 );
    vec3 sunDirection = l_light.m_v4Position.xyz;
    vec3 sunColor = l_light.m_v4Diffuse.rgb;
    vec3 reflection = normalize(reflect(-sunDirection, surfaceNormal));
    float direction = max(0.0, dot(eyeDirection, reflection));
    specularColor += pow(direction, shiny)*sunColor*spec;
    diffuseColor += max(dot(sunDirection, surfaceNormal),0.0)*sunColor*diffuse;
}

void main()
{

    // Noise + Lighting
    vec4 noise = getNoise(worldPosition.xz);
    vec3 surfaceNormal = normalize(noise.xzy*vec3(2.0, 1.0, 2.0));

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    vec3 worldToEye = c3d_v3CameraPosition-worldPosition;
    vec3 eyeDirection = normalize(worldToEye);
    sunLight(surfaceNormal, eyeDirection, 100.0, 2.0, 0.5, diffuse, specular);
    
    out_FragColor = vec4((diffuse+specular+vec3(0.1))*vec3(0.3, 0.5, 0.9), 1.0);
/*
    // Reflection
    float dist = length(worldToEye);

    vec2 screen = (projectedPosition.xy/projectedPosition.z + 1.0)*0.5;

    float distortionFactor = max(dist/100.0, 10.0);
    vec2 distortion = surfaceNormal.xz/distortionFactor;
    vec3 reflectionSample = vec3(texture2D(reflectionSampler, screen+distortion));
    
//    out_FragColor = vec4(c3d_v4MatDiffuse*(reflectionSample+vec3(0.1))*(diffuse+specular+0.3)*2.0, 1.0);
    
    // Reflectance
    float theta1 = max(dot(eyeDirection, surfaceNormal), 0.0);
    float rf0 = 0.02;
    float reflectance = rf0 + (1.0 - rf0)*pow((1.0 - theta1), 5.0);
//    vec3 albedo = mix(c3d_v4MatDiffuse*diffuse*0.3, (vec3(0.1)+reflectionSample*0.9+specular), reflectance);
    
//    out_FragColor = vec4(albedo, 1.0);

    // Scattering
    vec3 scatter = max(0.0, dot(surfaceNormal, eyeDirection))*vec3(0.0, 0.1, 0.07);
//    vec3 albedo = mix(c3d_v4MatDiffuse*light*0.3+scatter, (vec3(0.1)+reflectionSample), reflectance);
//    out_FragColor = vec4(albedo, 1.0);
    
    // Refraction
    vec3 refractionSample = vec3(texture2D(refractionSampler, screen - distortion));
    vec3 albedo = mix((scatter+(refractionSample*diffuse))*0.3, (vec3(0.1)+reflectionSample*0.9+specular), reflectance);
    out_FragColor = vec4(albedo, 1.0);
    
    // Absorbtion
    float depth = length(worldPosition-c3d_v3CameraPosition);
    float waterDepth = min(refractionSample.z-depth, 40.0);
    vec3 absorbtion = min((waterDepth/35.0)*vec3(2.0, 1.05, 1.0), vec3(1.0));
    vec3 refractionColor = mix(vec3(refractionSample)*0.5, c3d_v4MatDiffuse, absorbtion);
*/
}