attribute vec4 vertex;
varying vec2 uvIn;

void main() {
    uvIn = vertex.zw;
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
}
