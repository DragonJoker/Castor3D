#version 140
precision highp float;

in 			vec3 		vtx_normal;

out			vec4		out_FragColor;

void main()
{
	out_FragColor	= vec4( abs( vtx_normal.x ), abs( vtx_normal.y ), abs( vtx_normal.z ), 1.0 );
//	out_FragColor	= vec4( vtx_normal, 1.0 );
}
