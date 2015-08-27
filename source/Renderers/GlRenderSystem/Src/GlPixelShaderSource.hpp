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
#ifndef ___GL_PIXEL_SHADER_SOURCCE_H___
#define ___GL_PIXEL_SHADER_SOURCCE_H___

#include "GlShaderSource.hpp"

namespace GlRender
{
	namespace GLSL
	{
		class PixelShaderSource
		{
		protected:
			Castor::String m_strPixelMainDeclarations;
			Castor::String m_strPixelMainLightsLoop;
			Castor::String m_strPixelMainLightsLoopAfterLightDir;
			Castor::String m_strPixelMainLightsLoopEnd;
			Castor::String m_strPixelMainEnd;

		public:
			inline Castor::String const & GetPixelMainDeclarations()const
			{
				return m_strPixelMainDeclarations;
			}
			inline Castor::String const & GetPixelMainLightsLoop()const
			{
				return m_strPixelMainLightsLoop;
			}
			inline Castor::String const & GetPixelMainLightsLoopAfterLightDir()const
			{
				return m_strPixelMainLightsLoopAfterLightDir;
			}
			inline Castor::String const & GetPixelMainLightsLoopEnd()const
			{
				return m_strPixelMainLightsLoopEnd;
			}
			inline Castor::String const & GetPixelMainEnd()const
			{
				return m_strPixelMainEnd;
			}

			PixelShaderSource()
			{
				m_strPixelMainDeclarations =
					cuT(	"void main()\n" )
					cuT(	"{\n" )
					cuT(	"	Light l_light;\n" )
					cuT(	"	vec3 l_v3LightDir;\n" )
					cuT(	"	float l_fAttenuation;\n" )
					cuT(	"	float l_fLambert;\n" )
					cuT(	"	float l_fSpecular;\n" )
					cuT(	"	vec3 l_v3Normal = normalize( vec3( vtx_normal.x, vtx_normal.y, vtx_normal.z ) );\n" )
					cuT(	"	vec3 l_v3Ambient = vec3( 0, 0, 0 );\n" )
					cuT(	"	vec3 l_v3Diffuse = vec3( 0, 0, 0 );\n" )
					cuT(	"	vec3 l_v3Specular = vec3( 0, 0, 0 );\n" )
					cuT(	"	vec3 l_v3TmpAmbient = vec3( 0, 0, 0 );\n" )
					cuT(	"	vec3 l_v3TmpDiffuse = vec3( 0, 0, 0 );\n" )
					cuT(	"	vec3 l_v3TmpSpecular = vec3( 0, 0, 0 );\n" )
					//cuT(	"	vec3 l_v3EyeVec = normalize( vec3( vtx_vertex.x, vtx_vertex.y, vtx_vertex.z ) );\n" )
					cuT(	"	vec3 l_v3EyeVec = normalize( c3d_v3CameraPosition - vec3( vtx_vertex.x, vtx_vertex.y, vtx_vertex.z ) );\n" )
					cuT(	"	float l_fAlpha = c3d_fMatOpacity;\n" )
					cuT(	"	float l_fShininess = c3d_fMatShininess;\n" )
					cuT(	"	vec3 l_v3MatSpecular = c3d_v4MatSpecular.xyz;\n" )
					cuT(	"	vec3 l_v3Emissive = c3d_v4MatEmissive.xyz;\n" );

				m_strPixelMainLightsLoop =
					cuT(	"	for( int i = 0; i < c3d_iLightsCount; i++ )\n" )
					cuT(	"	{\n" )
					cuT(	"		l_light = GetLight( i );\n" )
					cuT(	"		vec4 l_v4LightDir = ComputeLightDirection( l_light, vtx_vertex, <pxlin_mtxModelView> );\n" )
					cuT(	"		l_v3LightDir = l_v4LightDir.xyz;\n" )
					cuT(	"		l_fAttenuation = l_v4LightDir.w;\n" );
				m_strPixelMainLightsLoopAfterLightDir =
					cuT(	"		l_fLambert = max( 0.0, dot( l_v3Normal, l_v3LightDir ) );\n" )
					//cuT(	"		l_fLambert = dot( l_v3Normal, -l_v3LightDir );\n" )
					cuT(	"		l_v3MatSpecular = c3d_v4MatSpecular.xyz;\n" )
					cuT(	"		float l_fFresnel = ComputeFresnel( l_fLambert, l_v3LightDir, l_v3Normal, l_v3EyeVec, l_fShininess, l_v3MatSpecular );\n" )
					cuT(	"		l_v3TmpAmbient = ( l_light.m_v4Ambient.xyz * l_light.m_v4Ambient.w * c3d_v4MatAmbient.xyz );\n" )
					cuT(	"		l_v3TmpDiffuse = ( l_light.m_v4Diffuse.xyz * l_light.m_v4Diffuse.w * c3d_v4MatDiffuse.xyz * l_fLambert ) / l_fAttenuation;\n" )
					cuT(	"		l_v3TmpSpecular = ( l_light.m_v4Specular.xyz * l_light.m_v4Specular.w * l_v3MatSpecular * l_fFresnel ) / l_fAttenuation;\n" );
				m_strPixelMainLightsLoopEnd =
					cuT(	"		l_v3Ambient += l_v3TmpAmbient;\n" )
					cuT(	"		l_v3Diffuse += l_v3TmpDiffuse;\n" )
					cuT(	"		l_v3Specular += l_v3TmpSpecular;\n" )
					cuT(	"	}\n" );

				m_strPixelMainEnd =
					cuT(	"}\n" );
			}
		};
	}

	static GLSL::PixelShaderSource pixelShaderSource;
}

#endif
