attribute vec4 vertex;

varying vec2 U; // sky map texture coordinates (world space stereographic projection)

void main() {
    gl_Position = vertex;
    U = vertex.xy * 1.1;
}
