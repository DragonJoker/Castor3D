uniform sampler2D spectrum_1_2_Sampler;
uniform sampler2D spectrum_3_4_Sampler;

uniform float FFT_SIZE;

uniform vec4 INVERSE_GRID_SIZES;

uniform float t;

varying in vec2 uv;

vec2 getSpectrum(float k, vec2 s0, vec2 s0c) {
    float w = sqrt(9.81 * k * (1.0 + k * k / (370.0 * 370.0)));
    float c = cos(w * t);
    float s = sin(w * t);
    return vec2((s0.x + s0c.x) * c - (s0.y + s0c.y) * s, (s0.x - s0c.x) * s + (s0.y - s0c.y) * c);
}

vec2 i(vec2 z) {
    return vec2(-z.y, z.x); // returns i times z (complex number)
}

void main() {
    vec2 st = floor(uv * FFT_SIZE) / FFT_SIZE;
    float x = uv.x > 0.5 ? st.x - 1.0 : st.x;
    float y = uv.y > 0.5 ? st.y - 1.0 : st.y;

    vec4 s12 = texture2DLod(spectrum_1_2_Sampler, uv, 0.0);
    vec4 s34 = texture2DLod(spectrum_3_4_Sampler, uv, 0.0);
    vec4 s12c = texture2DLod(spectrum_1_2_Sampler, vec2(1.0 + 0.5 / FFT_SIZE) - st, 0.0);
    vec4 s34c = texture2DLod(spectrum_3_4_Sampler, vec2(1.0 + 0.5 / FFT_SIZE) - st, 0.0);

    vec2 k1 = vec2(x, y) * INVERSE_GRID_SIZES.x;
    vec2 k2 = vec2(x, y) * INVERSE_GRID_SIZES.y;
    vec2 k3 = vec2(x, y) * INVERSE_GRID_SIZES.z;
    vec2 k4 = vec2(x, y) * INVERSE_GRID_SIZES.w;

    float K1 = length(k1);
    float K2 = length(k2);
    float K3 = length(k3);
    float K4 = length(k4);

    float IK1 = K1 == 0.0 ? 0.0 : 1.0 / K1;
    float IK2 = K2 == 0.0 ? 0.0 : 1.0 / K2;
    float IK3 = K3 == 0.0 ? 0.0 : 1.0 / K3;
    float IK4 = K4 == 0.0 ? 0.0 : 1.0 / K4;

    vec2 h1 = getSpectrum(K1, s12.xy, s12c.xy);
    vec2 h2 = getSpectrum(K2, s12.zw, s12c.zw);
    vec2 h3 = getSpectrum(K3, s34.xy, s34c.xy);
    vec2 h4 = getSpectrum(K4, s34.zw, s34c.zw);

    gl_FragData[0] = vec4(h1 + i(h2), h3 + i(h4));
    gl_FragData[1] = vec4(i(k1.x * h1) - k1.y * h1, i(k2.x * h2) - k2.y * h2);
    gl_FragData[2] = vec4(i(k3.x * h3) - k3.y * h3, i(k4.x * h4) - k4.y * h4);
    gl_FragData[3] = gl_FragData[1] * vec4(IK1, IK1, IK2, IK2);
    gl_FragData[4] = gl_FragData[2] * vec4(IK3, IK3, IK4, IK4);
}
