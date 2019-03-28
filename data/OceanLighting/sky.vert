uniform mat4 screenToCamera;
uniform mat4 cameraToWorld;

attribute vec4 vertex;
varying vec3 viewDir;

void main() {
    viewDir = (cameraToWorld * vec4((screenToCamera * vertex).xyz, 0.0)).xyz;
    gl_Position = vec4(vertex.xy, 0.9999999, 1.0);
}
