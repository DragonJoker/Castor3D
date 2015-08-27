#version 150
precision highp float;

uniform	mat4	ProjectionMatrix;
uniform	mat4	ModelMatrix;
uniform	mat4	ViewMatrix;
uniform	mat3	NormalMatrix;

in 		vec3 	normal;
in 		vec3 	tangent;
in 		vec4 	vertex;
in 		vec2 	texture;

/*
out 	vec2 	vtx_texture;
out 	vec3 	vtx_direction;
out		vec3 	vtx_vertex;
out		vec3 	vtx_normal;
out		vec3 	vtx_binormal;
out		vec3 	vtx_tangent;
out		mat4	vtx_mtxModelView;

void main()
{
	vec3 binormal	= cross( tangent, normal );
	vtx_normal		= normalize( NormalMatrix * normal		);
	vtx_tangent 	= normalize( NormalMatrix * tangent		);
	vtx_binormal 	= normalize( NormalMatrix * binormal	);
	
	mat3 TBN_Matrix;
	TBN_Matrix[0] =  vtx_tangent;
	TBN_Matrix[1] =  vtx_binormal;
	TBN_Matrix[2] =  vtx_normal;
	
	vtx_texture 		= texture;
	vtx_vertex 			= vec3(ModelViewMatrix * vertex);
	vtx_mtxModelView	= ModelViewMatrix;
	vtx_direction		= (-vertex.xyz) * TBN_Matrix;
	
	gl_Position 		= ProjectionModelViewMatrix * vertex;
}
*/

out		vec4 	position;
out		vec4	textureCoordinates;
out		mat3 	localSurface2World;
out		vec3	viewDirInScaledSurfaceCoords;
out		vec3	_WorldSpaceCameraPos;
out		mat4	vtx_mtxModelView;
out		mat4	vtx_mtxModelViewProjection;

void main()
{
	vtx_mtxModelView				= ViewMatrix * ModelMatrix;
	vtx_mtxModelViewProjection 		= ProjectionMatrix * vtx_mtxModelView;
	_WorldSpaceCameraPos 			= (vtx_mtxModelViewProjection * vec4( 0.0, 0.0, 0.0, 0.0 )).xyz;
	mat4 ModelViewMatrixInverse		= inverse( vtx_mtxModelView );
	localSurface2World[0] 			= normalize( vec3( vtx_mtxModelView * vec4( tangent, 0.0 ) ) );
	localSurface2World[2] 			= normalize( vec3( vec4( normal, 0.0 ) * ModelViewMatrixInverse ) );
	localSurface2World[1] 			= normalize( cross( localSurface2World[2], localSurface2World[0] ) );

	vec3 binormal 					= cross( tangent, normal ); 
	   // appropriately scaled tangent and binormal 
	   // to map distances from object space to texture space

	vec3 viewDirInObjectCoords 		= vec3( ModelViewMatrixInverse * vec4( _WorldSpaceCameraPos, 1.0) - vertex );
	mat3 localSurface2ScaledObject 	= mat3( tangent, binormal, normal);
	   // vectors are orthogonal
	viewDirInScaledSurfaceCoords 	= viewDirInObjectCoords * localSurface2ScaledObject; 
	   // we multiply with the transpose to multiply 
	   // with the "inverse" (apart from the scaling)

	position 						= vtx_mtxModelView * vertex;
	textureCoordinates 				= vec4( texture, 0.0, 0.0 );
	gl_Position 					= ProjectionMatrix * vtx_mtxModelView * vertex;
}
