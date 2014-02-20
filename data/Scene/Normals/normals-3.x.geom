#version 150
#extension GL_EXT_geometry_shader4: enable

uniform mat4 c3d_mtxProjectionModelView;
uniform int p_bShowNormals;
uniform int p_bShowTangents;
uniform int p_bShowBitangents;
uniform vec3 c3d_v3CameraPosition;

in vec4 vtx_normal[3];
in vec4 vtx_tangent[3];
in vec4 vtx_bitangent[3];
out vec4 geo_color;

void main()
{
    // On  dessine les wireframes
    for( int i = 0 ; i < gl_VerticesIn ; i++ )
    {
        gl_Position = c3d_mtxProjectionModelView * gl_PositionIn[i];
        geo_color = vec4( 1.0, 1.0, 1.0, 1.0 );
		geo_color.rgb /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
        EmitVertex();
    }
	gl_Position = c3d_mtxProjectionModelView * gl_PositionIn[0];
	geo_color = vec4( 1.0, 1.0, 1.0, 1.0 );
	geo_color.rgb /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
	EmitVertex();
    EndPrimitive();

	if( p_bShowNormals != 0 )
	{
		// on dessine les vecteurs normaux
		for( int i = 0 ; i < gl_VerticesIn ; i++ )
		{
			gl_Position = c3d_mtxProjectionModelView * gl_PositionIn[i];
			geo_color = vec4( 1.0, 0.0, 0.0, 1.0 );
			geo_color.a /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
			EmitVertex();

			gl_Position = c3d_mtxProjectionModelView * (gl_PositionIn[i] + (vtx_normal[i] * 5.0));
			geo_color = vec4( 1.0, 0.0, 0.0, 1.0 );
			geo_color.a /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
			EmitVertex();

			EndPrimitive();
		}
	}

	if( p_bShowTangents != 0 )
	{
		// on dessine les vecteurs tangentes
		for( int i = 0 ; i < gl_VerticesIn ; i++ )
		{
			gl_Position = c3d_mtxProjectionModelView * gl_PositionIn[i];
			geo_color = vec4( 0.0, 1.0, 0.0, 1.0 );
			geo_color.a /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
			EmitVertex();

			gl_Position = c3d_mtxProjectionModelView * (gl_PositionIn[i] + (vtx_tangent[i] * 5.0));
			geo_color = vec4( 0.0, 1.0, 0.0, 1.0 );
			geo_color.a /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
			EmitVertex();

			EndPrimitive();
		}
	}

	if( p_bShowBitangents != 0 )
	{
		// on dessine les vecteurs bitangentes
		for( int i = 0 ; i < gl_VerticesIn ; i++ )
		{
			gl_Position = c3d_mtxProjectionModelView * gl_PositionIn[i];
			geo_color = vec4( 0.0, 0.0, 1.0, 1.0 );
			geo_color.a /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
			EmitVertex();

			gl_Position = c3d_mtxProjectionModelView * (gl_PositionIn[i] + (vtx_bitangent[i] * 5.0));
			geo_color = vec4( 0.0, 0.0, 1.0, 1.0 );
			geo_color.a /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
			EmitVertex();

			EndPrimitive();
		}
	}
}
