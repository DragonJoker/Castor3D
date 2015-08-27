attribute vec3 vertex;

uniform mat4 c3d_mtxModelView;
uniform mat4 c3d_mtxProjection;

varying vec3 worldPosition;
varying vec4 projectedPosition;

void main()
{
    vec4 worldPosition4 = c3d_mtxModelView * vec4(vertex, 1.0);
    worldPosition = vec3(worldPosition4);
    projectedPosition = gl_Position = c3d_mtxProjection*worldPosition4;
}
