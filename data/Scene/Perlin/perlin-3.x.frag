#version 140

uniform sampler2D in_texture;

in vec2 vtx_texture;

out vec4 out_FragColor;

float rand( vec2 co)
{
	return fract( sin( dot( co.xy , vec2( 12.9898, 78.233))) * 43758.5453);
}

float noise( vec2 co)
{
	return rand( floor( co * 128.0));
}

vec4 rand2( vec2 A, vec2 B, vec2 C, vec2 D)
{
	vec2 s = vec2( 12.9898, 78.233);
	vec4 tmp = vec4( dot( A, s), dot( B, s), dot( C, s), dot( D, s));
	return fract( tan( tmp) * 43758.5453);
}

float noise2( vec2 coord, float d)
{ 
	vec2 C[4];
	float d1 = 1.0 / d;
	C[0] = floor( coord * d) * d1;
	C[1] = C[0] + vec2( d1, 0.0);
	C[2] = C[0] + vec2( d1, d1);
	C[3] = C[0] + vec2( 0.0, d1);
	vec2 p = fract( coord * d);
	vec2 q = 1.0 - p;
	vec4 w = vec4( q.x * q.y, p.x * q.y, p.x * p.y, q.x * p.y);
	return dot( vec4( rand2( C[0], C[1], C[2], C[3])), w);
}
 
void main()
{
//	out_FragColor = vec4( noise( vtx_texture.st));
	float level= -log2( min( length( dFdx( vtx_texture.st)), length( dFdy( vtx_texture.st))));
	level = min( level, 13.0); //limit the level to avoid slowness
	float n = 0.5;

	for(int i = 3 ; i < level ; i++)
	{ //3 is the lowest noise level
		n += 0.12 * noise2( vtx_texture.xy, exp2( i));
	}

	out_FragColor = vec4( n);
}