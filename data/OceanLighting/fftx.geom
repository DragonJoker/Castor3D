#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_geometry_shader4 : enable

uniform int nLayers;

varying in vec2 uvIn[];
varying vec2 uv;

void main() {
    for (int i = 0; i < nLayers; ++i) {
        gl_Layer = i;
        gl_PrimitiveID = i;
        gl_Position = gl_PositionIn[0];
        uv = uvIn[0];
        EmitVertex();
        gl_Position = gl_PositionIn[1];
        uv = uvIn[1];
        EmitVertex();
        gl_Position = gl_PositionIn[2];
        uv = uvIn[2];
        EmitVertex();
        EndPrimitive();
    }
}
