uniform vec3 worldCamera;
uniform vec3 worldSunDir;

in vec3 worldP;
out vec4 out_FragColor;

void main() {
    out_FragColor = cloudColor(worldP, worldCamera, worldSunDir);
    out_FragColor.rgb = hdr(out_FragColor.rgb);
}
