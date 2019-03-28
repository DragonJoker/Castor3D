uniform mat4 worldToScreen;

attribute vec4 vertex;
varying vec3 worldP;

void main() {
    gl_Position = worldToScreen * vec4(vertex.xyz, 1.0);
    worldP = vertex.xyz;
}

