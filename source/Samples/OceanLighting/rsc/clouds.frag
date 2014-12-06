uniform vec3 worldCamera;
uniform vec3 worldSunDir;

varying vec3 worldP;

void main() {
    gl_FragColor = cloudColor(worldP, worldCamera, worldSunDir);
    gl_FragColor.rgb = hdr(out_FragColor.rgb);
}
