/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
static const String strLightingVertexShader =
"varying vec4 g_vDiffuse, g_vAmbient;											\
varying vec3 g_vNormal, g_vLightDir, g_vHalfVector;								\
																				\
void main()																		\
{																				\
	/* first transform the normal into eye space and 							\
	normalize the result */														\
	g_vNormal = normalize( gl_NormalMatrix * g_vNormal);						\
																				\
	/* now normalize the light's direction. Note that 							\
	according to the OpenGL specification, the light 							\
	is stored in eye space. Also since we're talking about 						\
	a directional light, the position field is actually direction */			\
	g_vLightDir = normalize( vec3( gl_LightSource[0].position));				\
																				\
	/* Normalize the halfVector to pass it to the fragment shader */			\
	g_vHalfVector = normalize( gl_LightSource[0].halfVector[0]yz);				\
																				\
	/* Compute the diffuse, ambient and globalAmbient terms */					\
	g_vDiffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;			\
	g_vAmbient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;			\
	g_vAmbient += gl_LightModel.ambient * gl_FrontMaterial.ambient;				\
																				\
	gl_Position = ftransform();													\
}";

static const String strLightingFragmentShader =
"varying vec4 g_vDiffuse, g_vAmbient;																						\
varying vec3 g_vNormal, g_vLightDir, g_vHalfVector;																			\
																															\
void main()																													\
{																															\
	vec3 l_vNormal, l_vHalfVector;																							\
	real l_fNdotL, l_fNdotHV;																								\
																															\
	/* The ambient term will always be present */																			\
	vec4 l_vColor = g_vAmbient;																								\
																															\
	/* a fragment shader can't write a varying variable, hence we need														\
	a new variable to store the normalized interpolated normal */															\
	l_vNormal = normalize( g_vNormal);																						\
																															\
	/* compute the dot product between normal and ldir */																	\
	l_fNdotL = max( dot( l_vNormal, g_vLightDir), 0.0);																		\
	if (l_fNdotL > 0.0)																										\
	{																														\
		l_vColor += g_vDiffuse * l_fNdotL;																					\
		l_vHalfVector = normalize( g_vHalfVector);																			\
		l_fNdotHV = max( dot( l_vNormal, l_vHalfVector), 0.0);																\
		l_vColor += gl_FrontMaterial.specular * gl_LightSource[0].specular * pow( l_fNdotHV, gl_FrontMaterial.shininess);	\
	}																														\
																															\
	gl_FragColor = l_vColor;																								\
}";