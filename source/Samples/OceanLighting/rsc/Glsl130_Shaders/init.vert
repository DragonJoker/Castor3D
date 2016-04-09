in vec4 vertex;
out vec2 uv;

void main() {
    uv = vertex.zw;
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
}
