uniform mat4 worldToScreen;

in vec4 vertex;
out vec3 worldP;

void main() {
    gl_Position = worldToScreen * vec4(vertex.xyz, 1.0);
    worldP = vertex.xyz;
}

