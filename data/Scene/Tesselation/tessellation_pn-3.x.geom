#version 150
#extension GL_EXT_geometry_shader4: enable

#define TRIANGLES	0

uniform mat4 c3d_mtxProjectionModelView;

in vec3 vtx_normal[3];
in vec3 vtx_tangent[3];
in vec3 vtx_bitangent[3];
in vec2 vtx_texture[3];

out vec4 geo_normal;
out vec4 geo_tangent;
out vec4 geo_bitangent;
out vec2 geo_texture;

struct Vertex
{
	vec4 Position;
	vec4 Normal;
	vec4 Tangent;
	vec4 Bitangent;
	vec2 Texture;
};

Vertex InitVertex( vec4 p_position, vec3 p_normal, vec3 p_tangent, vec3 p_bitangent, vec2 p_texture )
{
	Vertex l_return;
	l_return.Position	= p_position;
	l_return.Normal		= vec4 ( p_normal,		0.0 );
	l_return.Tangent	= vec4 ( p_tangent,		0.0 );
	l_return.Bitangent	= vec4 ( p_bitangent,	0.0 );
	l_return.Texture	= p_texture;
	return l_return;
}

Vertex CreateVertex( const Vertex p_a, const Vertex p_b )
{
	Vertex l_return;
	l_return.Position	= (p_a.Position		+ p_b.Position)		/ 2.0;
	l_return.Normal		= vec4( normalize( p_a.Normal.xyz		+ p_b.Normal.xyz	), 0.0 );
	l_return.Tangent	= vec4( normalize( p_a.Tangent.xyz		+ p_b.Tangent.xyz	), 0.0 );
	l_return.Bitangent	= vec4( normalize( p_a.Bitangent.xyz	+ p_b.Bitangent.xyz	), 0.0 );
	l_return.Texture	= (p_a.Texture		+ p_b.Texture)		/ 2.0;
	return l_return;
}

void EmitTriangle( Vertex p_a, Vertex p_b, Vertex p_c  )
{
	gl_Position		= c3d_mtxProjectionModelView * p_a.Position;
	geo_normal		= c3d_mtxProjectionModelView * p_a.Normal;
	geo_tangent		= c3d_mtxProjectionModelView * p_a.Tangent;
	geo_bitangent	= c3d_mtxProjectionModelView * p_a.Bitangent;
	geo_texture		= p_a.Texture;
	EmitVertex();
	gl_Position		= c3d_mtxProjectionModelView * p_b.Position;
	geo_normal		= c3d_mtxProjectionModelView * p_b.Normal;
	geo_tangent		= c3d_mtxProjectionModelView * p_b.Tangent;
	geo_bitangent	= c3d_mtxProjectionModelView * p_b.Bitangent;
	geo_texture		= p_b.Texture;
	EmitVertex();
	gl_Position		= c3d_mtxProjectionModelView * p_c.Position;
	geo_normal		= c3d_mtxProjectionModelView * p_c.Normal;
	geo_tangent		= c3d_mtxProjectionModelView * p_c.Tangent;
	geo_bitangent	= c3d_mtxProjectionModelView * p_c.Bitangent;
	geo_texture		= p_c.Texture;
	EmitVertex();
    EndPrimitive();
}

void EmitLines( Vertex p_a, Vertex p_b, Vertex p_c  )
{
	gl_Position		= c3d_mtxProjectionModelView * p_a.Position;
	geo_normal		= c3d_mtxProjectionModelView * p_a.Normal;
	geo_tangent		= c3d_mtxProjectionModelView * p_a.Tangent;
	geo_bitangent	= c3d_mtxProjectionModelView * p_a.Bitangent;
	geo_texture		= p_a.Texture;
	EmitVertex();
	gl_Position		= c3d_mtxProjectionModelView * p_b.Position;
	geo_normal		= c3d_mtxProjectionModelView * p_b.Normal;
	geo_tangent		= c3d_mtxProjectionModelView * p_b.Tangent;
	geo_bitangent	= c3d_mtxProjectionModelView * p_b.Bitangent;
	geo_texture		= p_b.Texture;
	EmitVertex();
	gl_Position		= c3d_mtxProjectionModelView * p_c.Position;
	geo_normal		= c3d_mtxProjectionModelView * p_c.Normal;
	geo_tangent		= c3d_mtxProjectionModelView * p_c.Tangent;
	geo_bitangent	= c3d_mtxProjectionModelView * p_c.Bitangent;
	geo_texture		= p_c.Texture;
	EmitVertex();
	gl_Position		= c3d_mtxProjectionModelView * p_a.Position;
	geo_normal		= c3d_mtxProjectionModelView * p_a.Normal;
	geo_tangent		= c3d_mtxProjectionModelView * p_a.Tangent;
	geo_bitangent	= c3d_mtxProjectionModelView * p_a.Bitangent;
	geo_texture		= p_a.Texture;
	EmitVertex();
    EndPrimitive();
}

// Tesselating using following subdivision pattern
//		C
//	   /\
//	 F/__\E
//	 /\  /\
// A/__\/__\B
//	   D
// Each new vertex is then repositionned using the 2 adjacent vertices normals

void main()
{
	// On récupère les positions des points
	Vertex a = InitVertex( gl_PositionIn[0], vtx_normal[0], vtx_tangent[0], vtx_bitangent[0], vtx_texture[0] );
	Vertex b = InitVertex( gl_PositionIn[1], vtx_normal[1], vtx_tangent[1], vtx_bitangent[1], vtx_texture[1] );
	Vertex c = InitVertex( gl_PositionIn[2], vtx_normal[2], vtx_tangent[2], vtx_bitangent[2], vtx_texture[2] );

	Vertex d = CreateVertex( a, b );
	Vertex e = CreateVertex( b, c );
	Vertex f = CreateVertex( c, a );

    // On  dessine les triangles
#if TRIANGLES
	EmitTriangle( d, a, f );
	EmitTriangle( b, d, e );
	EmitTriangle( e, f, c );
	EmitTriangle( e, d, f );
#else
	EmitLines( d, a, f );
	EmitLines( b, d, e );
	EmitLines( e, f, c );
	EmitLines( e, d, f );
#endif
}
