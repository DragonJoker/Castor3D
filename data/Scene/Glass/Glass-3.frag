#version 150

in vec4 geo_normal;
in vec4 geo_tangent;
in vec4 geo_bitangent;
in vec2 geo_texture;

uniform sampler2D c3d_mapDiffuse;

out vec4 out_FragColor;

void main()
{
	out_FragColor = texture2D( c3d_mapDiffuse, geo_texture );
//    out_FragColor = vec4( geo_normal.xyz, 1.0 );
}
