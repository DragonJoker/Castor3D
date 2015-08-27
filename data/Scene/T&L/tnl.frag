uniform sampler2D DiffuseMap;

varying vec3 eyeVertex;
varying vec3 eyeNormal;
varying vec2 texCoord;

varying vec3 lightDir[8];
varying vec4 lightDiff[8];
varying vec4 lightSpec[8];
varying vec4 lightAmbt[8];

varying vec4 ex_MatDiffuse;
varying vec4 ex_MatAmbient;
varying vec4 ex_MatSpecular;
varying float ex_MatShininess;

void main()
{
	vec4 final_color = texture2D( DiffuseMap, texCoord);
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

	gl_FragColor = final_color;
}
