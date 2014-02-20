#version 130
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D butterflySampler;
uniform sampler2DArray imgSampler; // 2 complex inputs (= 4 values) per layer

uniform float pass;

in vec2 uv;

out vec4 out_FragColor;

// performs two FFTs on two inputs packed in a single texture
// returns two results packed in a single vec4
vec4 fft2(int layer, vec2 i, vec2 w) {
    vec4 input1 = textureLod(imgSampler, vec3(uv.x, i.x, layer), 0.0);
    vec4 input2 = textureLod(imgSampler, vec3(uv.x, i.y, layer), 0.0);
    float res1x = w.x * input2.x - w.y * input2.y;
    float res1y = w.y * input2.x + w.x * input2.y;
    float res2x = w.x * input2.z - w.y * input2.w;
    float res2y = w.y * input2.z + w.x * input2.w;
    return input1 + vec4(res1x, res1y, res2x, res2y);
}

void main() {
    vec4 data = textureLod(butterflySampler, vec2(uv.y, pass), 0.0);
    vec2 i = data.xy;
    vec2 w = data.zw;

    out_FragColor = fft2(gl_PrimitiveID, i, w);
}
