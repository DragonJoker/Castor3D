#version 140

uniform sampler2D c3d_mapDiffuse;

in vec3 eyeVertex;
in vec3 eyeNormal;
in vec2 texCoord;

in vec3 lightDir[8];
in vec4 lightDiff[8];
in vec4 lightSpec[8];
in vec4 lightAmbt[8];

in vec4 ex_MatDiffuse;
in vec4 ex_MatAmbient;
in vec4 ex_MatSpecular;
in float ex_MatShininess;

out vec4 out_FragColor;

void main()
{
	vec4 final_color = texture2D( c3d_mapDiffuse, texCoord);
	vec3 N = normalize( eyeNormal);
	vec3 E = normalize( eyeVertex);
	int i;

	for (i = 0 ; i < 3 ; i++)
	{
		vec3 L = normalize( lightDir[i]);
		vec3 R = reflect( L, N);
		float lambertTerm = max( dot( N, L), 0.0);

		vec4 l_diffuse = lightDiff[i] * ex_MatDiffuse * lambertTerm;
		vec4 l_specular = ex_MatSpecular * lightSpec[i] * pow( max( dot( R, E), 0.0), ex_MatShininess);

		final_color += l_diffuse + l_specular;
	}

	out_FragColor = final_color;
}
