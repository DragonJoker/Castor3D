/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___GL_DEFERRED_SHADER_SOURCCE_H___
#define ___GL_DEFERRED_SHADER_SOURCCE_H___

#include "GlShaderSource.hpp"

namespace GlRender
{
	namespace GLSL
	{
		class DeferredShaderSource
			: public ShaderSource
		{
		protected:
			Castor::String m_strGSPixelDeclarations;
			Castor::String m_strGSPixelMainDeclarations;
			Castor::String m_strGSPixelMainLightsLoopAfterLightDir;
			Castor::String m_strGSPixelMainEnd;
			Castor::String m_strLSPixelProgram;

		public:
			inline Castor::String const & GetGSPixelDeclarations()
			{
				return m_strGSPixelDeclarations;
			}
			inline Castor::String const & GetGSPixelMainDeclarations()
			{
				return m_strGSPixelMainDeclarations;
			}
			inline Castor::String const & GetGSPixelMainLightsLoopAfterLightDir()
			{
				return m_strGSPixelMainLightsLoopAfterLightDir;
			}
			inline Castor::String const & GetGSPixelMainEnd()
			{
				return m_strGSPixelMainEnd;
			}
			inline Castor::String const & GetLSPixelProgram()
			{
				return m_strLSPixelProgram;
			}

			DeferredShaderSource()
			{
				m_strGSPixelDeclarations = cuT( "" );
				m_strGSPixelMainDeclarations =
					cuT(	"void main()\n" )
					cuT(	"{\n" )
					cuT(	"	vec4 l_v4Position = vec4( vtx_vertex.xyz, 1 );\n" )
					cuT(	"	vec4 l_v4Normal = vec4( vtx_normal, 1 );\n" )
					cuT(	"	vec4 l_v4Tangent = vec4( vtx_tangent, 1 );\n" )
					cuT(	"	vec4 l_v4Bitangent = vec4( vtx_bitangent, 1 );\n" )
					cuT(	"	vec4 l_v4Diffuse = vec4( 0, 0, 0, 0 );\n" )
					cuT(	"	vec4 l_v4Texture = vec4( vtx_texture.xy, 0, 1 );\n" )
					cuT(	"	vec4 l_v4Specular = vec4( c3d_v4MatSpecular.xyz, c3d_fMatShininess );\n" )
					cuT(	"	vec4 l_v4Emissive = c3d_v4MatEmissive;\n" )
					cuT(	"	float l_fAlpha = c3d_fMatOpacity;\n" );
				m_strGSPixelMainLightsLoopAfterLightDir =
					cuT(	"    l_v4Diffuse     = c3d_v4MatDiffuse;\n" );
				m_strGSPixelMainEnd =
					cuT(	"}\n" );
				m_strLSPixelProgram =
					GetPixelLights() +
					cuT(	"uniform sampler2D c3d_mapPosition;\n" )
					cuT(	"uniform sampler2D c3d_mapDiffuse;\n" )
					cuT(	"uniform sampler2D c3d_mapNormals;\n" )
					cuT(	"uniform sampler2D c3d_mapTangent;\n" )
					cuT(	"uniform sampler2D c3d_mapBitangent;\n" )
					cuT(	"uniform sampler2D c3d_mapSpecular;\n" )
					cuT(	"uniform sampler2D c3d_mapEmissive;\n" )
					cuT(	"in vec2 vtx_texture;\n" )
					cuT(	"void main( void )\n" )
					cuT(	"{\n" )
					cuT(	"	Light l_light;\n" )
					cuT(	"	vec4 l_v4Positions = <texture2D>( c3d_mapPosition, vtx_texture );\n" )
					cuT(	"	vec4 l_v4Diffuses = <texture2D>( c3d_mapDiffuse, vtx_texture );\n" )
					cuT(	"	vec4 l_v4Normals = <texture2D>( c3d_mapNormals, vtx_texture ) / 2.0;\n" )
					cuT(	"	vec4 l_v4Tangents = <texture2D>( c3d_mapTangent, vtx_texture );\n" )
					cuT(	"	vec4 l_v4Bitangents = <texture2D>( c3d_mapBitangent, vtx_texture );\n" )
					cuT(	"	vec4 l_v4Speculars = <texture2D>( c3d_mapSpecular, vtx_texture );\n" )
					cuT(	"	vec4 l_v4Emissives = <texture2D>( c3d_mapEmissive, vtx_texture );\n" )
					cuT(	"	float l_fShininess = l_v4Speculars.w;\n" )
					cuT(	"	vec3 l_v3Position = l_v4Positions.xyz;\n" )
					cuT(	"	vec3 l_v3Normal = l_v4Normals.xyz;\n" )
					cuT(	"	vec3 l_v3Bitangent = l_v4Bitangents.xyz;\n" )
					cuT(	"	vec3 l_v3Tangent = l_v4Tangents.xyz;\n" )
					cuT(	"	vec3 l_v3Specular = vec3( 0, 0, 0 );\n" )
					cuT(	"	vec3 l_v3Diffuse = vec3( 0, 0, 0 );\n" )
					cuT(	"	vec3 l_v3Emissive = l_v4Emissives.xyz;\n" )
					cuT(	"	vec3 l_v3TmpVec = -l_v3Position;\n" )
					cuT(	"	vec3 l_v3EyeVec;\n" )
					cuT(	"	vec3 l_v3HalfVector;\n" )
					cuT(	"	vec3 l_v3LightDir;\n" )
					cuT(	"	float l_fLambert;\n" )
					cuT(	"	float l_fSpecular;\n" )
					cuT(	"	float l_fSqrLength;\n" )
					cuT(	"	float l_fAttenuation;\n" )
					cuT(	"	vec3 l_v3TmpDiffuse;\n" )
					cuT(	"	vec3 l_v3TmpSpecular;\n" )
					cuT(	"	vec4 l_v4MatSpecular;\n" )
					cuT(	"	l_v3EyeVec.x = dot( l_v3TmpVec, l_v3Tangent );\n" )
					cuT(	"	l_v3EyeVec.y = dot( l_v3TmpVec, l_v3Bitangent );\n" )
					cuT(	"	l_v3EyeVec.z = dot( l_v3TmpVec, l_v3Normal );\n" )
					cuT(	"	l_v3EyeVec = normalize( l_v3EyeVec );\n" )
					cuT(	"	for( int i = 0; i < c3d_iLightsCount; i++ )\n" )
					cuT(	"	{\n" )
					cuT(	"		l_light = GetLight( i );\n" )
					cuT(	"		vec4 l_v4LightDir = BlinnPhongDir( l_light, l_v3Position, <pxlin_mtxModelView> );\n" )
					cuT(	"		l_v3LightDir = l_v4LightDir.xyz;\n" )
					cuT(	"		l_fAttenuation = l_v4LightDir.w;\n" )
					cuT(	"		Bump( l_v3Bitangent, l_v3Tangent, l_v3Normal, l_v3LightDir, l_fAttenuation );\n" )
					cuT(	"		l_fLambert = max( 0.0, dot( l_v3Normal, l_v3LightDir ) );\n" )
					cuT(	"		l_v4MatSpecular = l_v4Speculars;\n" )
					cuT(	"		float l_fFresnel = Fresnel( l_fLambert, l_v3LightDir, l_v3Normal, l_v3EyeVec, l_fShininess, l_v4MatSpecular );\n" )
					cuT(	"		l_v3TmpDiffuse = l_light.m_v4Diffuse.xyz * l_v4Diffuses.xyz * l_fLambert;\n" )
					cuT(	"		l_v3TmpSpecular = l_light.m_v4Specular.xyz * l_v4MatSpecular.xyz * l_fFresnel;\n" )
					cuT(	"		l_v3Diffuse += l_v3TmpDiffuse * l_fAttenuation;\n" )
					cuT(	"		l_v3Specular += l_v3TmpSpecular * l_fAttenuation;\n" )
					cuT(	"	}\n" )
					cuT(	"	pxl_v4FragColor = vec4( l_v3Emissive + l_v3Diffuse + l_v3Specular, 1 );\n" )
					cuT(	"}\n" );
			}
		};
	}

	GLSL::DeferredShaderSource deferredShaderSource;
}

#endif
