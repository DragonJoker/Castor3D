uniform vec3 worldCamera;
uniform vec3 worldSunDir;

varying in vec3 viewDir;

void main() {
    vec3 v = normalize(viewDir);
    vec3 sunColor = vec3(step(cos(3.1415926 / 180.0), dot(v, worldSunDir))) * SUN_INTENSITY;
    vec3 extinction;
    vec3 inscatter = skyRadiance(worldCamera + earthPos, v, worldSunDir, extinction);
    vec3 finalColor = sunColor * extinction + inscatter;
    gl_FragColor.rgb = hdr(finalColor);
    gl_FragColor.a = 1.0;
}
