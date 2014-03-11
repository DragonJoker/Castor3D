#version 140

in vec3 vertex;
 

layout( std140 ) uniform Matrices
{
    mat4 c3d_mtxProjection;
    mat4 c3d_mtxModel;
    mat4 c3d_mtxView;
    mat4 c3d_mtxModelView;
    mat4 c3d_mtxProjectionView;
    mat4 c3d_mtxProjectionModelView;
    mat4 c3d_mtxNormal;
    mat4 c3d_mtxTexture0;
    mat4 c3d_mtxTexture1;
    mat4 c3d_mtxTexture2;
    mat4 c3d_mtxTexture3;
    mat4 c3d_mtxBones[100];
};

out vec3 worldPosition;
out vec4 projectedPosition;

void main()
{
    vec4 worldPosition4 = c3d_mtxModelView * vec4(vertex, 1.0);
    worldPosition = vec3(worldPosition4);
    projectedPosition = gl_Position = c3d_mtxProjection*worldPosition4;
}
