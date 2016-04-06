#version 420

precision highp float;

layout( triangles ) in;
layout( line_strip ) out;
layout( max_vertices = 22 ) out;

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

layout( std140 ) uniform Config
{
	uniform int p_bShowNormals;
	uniform int p_bShowTangents;
	uniform int p_bShowBitangents;
	uniform vec3 c3d_v3CameraPosition;
};

in vec4 vtx_normal[3];
in vec4 vtx_tangent[3];
in vec4 vtx_bitangent[3];
out vec4 geo_color;

void main()
{
	// On  dessine les wireframes
	for( int i = 0 ; i < gl_in.length() ; i++ )
	{
		gl_Position = c3d_mtxProjectionModelView * gl_in[i].gl_Position;
		geo_color = vec4( 1.0, 1.0, 1.0, 1.0 );
		geo_color.rgb /= (distance(c3d_v3CameraPosition, gl_Position.xyz));
		geo_color.rgb = vec3( 1.0, 1.0, 1.0 ) - geo_color.rgb;
		geo_color.a /= distance(c3d_v3CameraPosition, gl_Position.xyz);
		EmitVertex();
	}
	gl_Position = c3d_mtxProjectionModelView * gl_in[0].gl_Position;
	geo_color = vec4( 1.0, 1.0, 1.0, 1.0 );
	geo_color.rgb /= (distance(c3d_v3CameraPosition, gl_Position.xyz));
	geo_color.rgb = vec3( 1.0, 1.0, 1.0 ) - geo_color.rgb;
	geo_color.a /= distance(c3d_v3CameraPosition, gl_Position.xyz);
	EmitVertex();
	EndPrimitive();

	if( p_bShowNormals != 0 )
	{
		// on dessine les vecteurs normaux
		for( int i = 0 ; i < gl_in.length() ; i++ )
		{
			gl_Position = c3d_mtxProjectionModelView * gl_in[i].gl_Position;
			geo_color = vec4( 1.0, 0.0, 0.0, 1.0 );
			geo_color /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
			geo_color.a /= distance(c3d_v3CameraPosition, gl_Position.xyz);
			EmitVertex();

			gl_Position = c3d_mtxProjectionModelView * (gl_in[i].gl_Position + (vtx_normal[i] * 5.0));
			geo_color = vec4( 1.0, 0.0, 0.0, 1.0 );
			geo_color /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
			geo_color.a /= distance(c3d_v3CameraPosition, gl_Position.xyz);
			EmitVertex();

			EndPrimitive();
		}
	}

	if( p_bShowTangents != 0 )
	{
		// on dessine les vecteurs tangentes
		for( int i = 0 ; i < gl_in.length() ; i++ )
		{
			gl_Position = c3d_mtxProjectionModelView * gl_in[i].gl_Position;
			geo_color = vec4( 0.0, 1.0, 0.0, 1.0 );
			geo_color /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
			EmitVertex();

			gl_Position = c3d_mtxProjectionModelView * (gl_in[i].gl_Position + (vtx_tangent[i] * 5.0));
			geo_color = vec4( 0.0, 1.0, 0.0, 1.0 );
			geo_color /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
			geo_color.a /= distance(c3d_v3CameraPosition, gl_Position.xyz);
			EmitVertex();

			EndPrimitive();
		}
	}

	if( p_bShowBitangents != 0 )
	{
		// on dessine les vecteurs bitangentes
		for( int i = 0 ; i < gl_in.length() ; i++ )
		{
			gl_Position = c3d_mtxProjectionModelView * gl_in[i].gl_Position;
			geo_color = vec4( 0.0, 0.0, 1.0, 1.0 );
			geo_color /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
			geo_color.a /= distance(c3d_v3CameraPosition, gl_Position.xyz);
			EmitVertex();

			gl_Position = c3d_mtxProjectionModelView * (gl_in[i].gl_Position + (vtx_bitangent[i] * 5.0));
			geo_color = vec4( 0.0, 0.0, 1.0, 1.0 );
			geo_color /= (distance(c3d_v3CameraPosition, gl_Position.xyz)/2);
			geo_color.a /= distance(c3d_v3CameraPosition, gl_Position.xyz);
			EmitVertex();

			EndPrimitive();
		}
	}
}
