#version 420
#extension GL_EXT_geometry_shader4: enable
precision highp float;
layout (triangles) in;
layout (line_strip) out;
layout (max_vertices = 16) out;

uniform mat4 c3d_mtxProjectionModelView;

out vec4 geo_color;

struct Triangle
{
	vec4 a;
	vec4 b;
	vec4 c;
};

Triangle InitTriangle( vec4 p_a, vec4 p_b, vec4 p_c )
{
	Triangle l_return;
	l_return.a = p_a;
	l_return.b = p_b;
	l_return.c = p_c;
	return l_return;
}

Triangle EmitLines( vec4 p_a, vec4 p_b, vec4 p_c )
{
	Triangle l_return = InitTriangle( p_a, p_b, p_c );
	gl_Position		= c3d_mtxProjectionModelView * p_a;
	geo_color		= vec4( 0.0, 0.0, 0.0, 0.0 );
	EmitVertex();
	gl_Position		= c3d_mtxProjectionModelView * p_b;
	geo_color		= vec4( 0.0, 0.0, 0.0, 0.0 );
	EmitVertex();
	gl_Position		= c3d_mtxProjectionModelView * p_c;
	geo_color		= vec4( 0.0, 0.0, 0.0, 0.0 );
	EmitVertex();
	gl_Position		= c3d_mtxProjectionModelView * p_a;
	geo_color		= vec4( 0.0, 0.0, 0.0, 0.0 );
	EmitVertex();
    EndPrimitive();
	return l_return;
}

// Tesselating using following subdivision pattern
//		C
//	   /\
//	 F/__\E
//	 /\  /\
// A/__\/__\B
//	   D
// Each new vertex is then repositionned using the 2 adjacent vertices normals

void Tessellate( Triangle p_tri )
{
	vec4 l_a = p_tri.a;
	vec4 l_b = p_tri.b;
	vec4 l_c = p_tri.c;
	vec4 l_d = (l_a + l_b) / 2.0;
	vec4 l_e = (l_b + l_c) / 2.0;
	vec4 l_f = (l_c + l_a) / 2.0;

	// On  dessine les triangles
	EmitLines( l_d, l_a, l_f );
	EmitLines( l_b, l_d, l_e );
	EmitLines( l_e, l_f, l_c );
	EmitLines( l_e, l_d, l_f );
}

void main()
{
	Tessellate( InitTriangle( gl_PositionIn[0], gl_PositionIn[1], gl_PositionIn[2] ) );
}
