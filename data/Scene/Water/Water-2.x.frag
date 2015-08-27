uniform vec3 c3d_v3CameraPosition;

varying vec3 worldPosition;
varying vec4 projectedPosition;

vec4 getNoise(vec2 uv)
{
    vec2 uv0 = (uv/103.0)+vec2(time/17.0, time/29.0);
    vec2 uv1 = uv/107.0-vec2(time/-19.0, time/31.0);
    vec2 uv2 = uv/vec2(897.0, 983.0)+vec2(time/101.0, time/97.0);
    vec2 uv3 = uv/vec2(991.0, 877.0)-vec2(time/109.0, time/-113.0);
    vec4 noise = (texture2D(normalSampler, uv0)) +
                 (texture2D(normalSampler, uv1)) +
                 (texture2D(normalSampler, uv2)) +
                 (texture2D(normalSampler, uv3));
    return noise*0.5-1.0;
}

void sunLight(const vec3 surfaceNormal, const vec3 eyeDirection, float shiny, float spec, float diffuse, inout vec3 diffuseColor, inout vec3 specularColor)
{
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
    
//    gl_FragColor = vec4((diffuse+specular+vec3(0.1))*vec3(0.3, 0.5, 0.9), 1.0);

    // Reflection
    float dist = length(worldToEye);

    vec2 screen = (projectedPosition.xy/projectedPosition.z + 1.0)*0.5;

    float distortionFactor = max(dist/100.0, 10.0);
    vec2 distortion = surfaceNormal.xz/distortionFactor;
    vec3 reflectionSample = vec3(texture2D(reflectionSampler, screen+distortion));
    
//    gl_FragColor = vec4(color*(reflectionSample+vec3(0.1))*(diffuse+specular+0.3)*2.0, 1.0);
    
    // Reflectance
    float theta1 = max(dot(eyeDirection, surfaceNormal), 0.0);
    float rf0 = 0.02;
    float reflectance = rf0 + (1.0 - rf0)*pow((1.0 - theta1), 5.0);
//    vec3 albedo = mix(color*diffuse*0.3, (vec3(0.1)+reflectionSample*0.9+specular), reflectance);
    
//    gl_FragColor = vec4(albedo, 1.0);

    // Scattering
    vec3 scatter = max(0.0, dot(surfaceNormal, eyeDirection))*vec3(0.0, 0.1, 0.07);
//    vec3 albedo = mix(color*light*0.3+scatter, (vec3(0.1)+reflectionSample), reflectance);
//    gl_FragColor = vec4(albedo, 1.0);
    
    // Refraction
    vec3 refractionSample = vec3(texture2D(refractionSampler, screen - distortion));
    vec3 albedo = mix((scatter+(refractionSample*diffuse))*0.3, (vec3(0.1)+reflectionSample*0.9+specular), reflectance);
    gl_FragColor = vec4(albedo, 1.0);
    
    // Absorbtion
    float depth = length(worldPosition-eye);
    float waterDepth = min(refractionSample.a-depth, 40.0);
    vec3 absorbtion = min((waterDepth/35.0)*vec3(2.0, 1.05, 1.0), vec3(1.0));
    vec3 refractionColor = mix(vec3(refractionSample)*0.5, color, absorbtion);

}