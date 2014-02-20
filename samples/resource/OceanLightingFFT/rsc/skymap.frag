
uniform vec3 sunDir;

in vec2 U; // sky map texture coordinates (world space stereographic projection)

out vec4 out_FragColor;

void main() {
    vec2 u = U;
    float l = dot(u, u);
    vec4 result = vec4(0.0);

    if (l <= 1.02) {
        if (l > 1.0) {
            u = u / l;
            l = 1.0 / l;
        }

        // inverse stereographic projection,
        // from skymap coordinates to world space directions
        vec3 r = vec3(2.0 * u, 1.0 - l) / (1.0 + l);

        vec3 extinction;
        result.rgb = skyRadiance(earthPos, r, sunDir, extinction);

#ifdef CLOUDS
        vec4 cloudL = cloudColorV(vec3(0.0), r, sunDir);
        result.rgb = cloudL.rgb * cloudL.a + result.rgb * (1.0 - cloudL.a);
#endif
    } else {
        // below horizon:
        // use average fresnel * average sky radiance
        // to simulate multiple reflections on waves

        const float avgFresnel = 0.17;
        result.rgb = skyIrradiance(earthPos.z, sunDir.z) / M_PI * avgFresnel;
    }

    out_FragColor = vec4( 1.0, 1.0, 0.0, 1.0 );//result;
}
