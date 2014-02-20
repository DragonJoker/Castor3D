attribute	vec4 	vertex;
attribute	vec3 	normal;

uniform		mat4 	c3d_mtxProjection;
uniform		mat4 	c3d_mtxModel;
uniform		mat4 	c3d_mtxView;

varying		vec3 	vtx_normal;

void main()
{
	if( normal.x < 0 || normal.y < 0 || normal.z < 0 )
	{
		vtx_normal = vec3( abs( normal.x ), abs( normal.y ), abs( normal.z ) );
	}
	else
	{
		vtx_normal = normal;
	}

	gl_Position	= c3d_mtxProjection * (c3d_mtxView * c3d_mtxModel) * vertex;
}
