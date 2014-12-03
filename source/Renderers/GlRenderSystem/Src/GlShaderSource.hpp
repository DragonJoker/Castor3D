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
#ifndef ___GL_SHADER_SOURCCE_H___
#define ___GL_SHADER_SOURCCE_H___

#include "GlRenderSystemPrerequisites.hpp"

namespace GlRender
{
	namespace GLSL
	{
		class ShaderSource
		{
		private:
			Castor::String m_strPixelLights;

		public:
			ShaderSource()
			{
				m_strPixelLights =
					cuT(	"struct Light\n" )
					cuT(	"{\n" )
					cuT(	"    vec4    m_v4Ambient;\n" )
					cuT(	"    vec4    m_v4Diffuse;\n" )
					cuT(	"    vec4    m_v4Specular;\n" )
					cuT(	"    vec4    m_v4Position;\n" )
					cuT(	"    int     m_iType;\n" )
					cuT(	"    vec3    m_v3Attenuation;\n" )
					cuT(	"    mat4    m_mtx4Orientation;\n" )
					cuT(	"    float   m_fExponent;\n" )
					cuT(	"    float   m_fCutOff;\n" )
					cuT(	"};\n" )
					cuT(	"Light GetLight( int p_iIndex )\n" )
					cuT(	"{\n" )
					cuT(	"    float   l_fFactor               = (float( p_iIndex ) * 0.01);\n" )
					cuT(	"    Light   l_lightReturn;\n" )
					cuT(	"    l_lightReturn.m_v4Ambient       = <texture1D>( c3d_sLights, l_fFactor + (0 * 0.001) + 0.0005 );\n" )
					cuT(	"    l_lightReturn.m_v4Diffuse       = <texture1D>( c3d_sLights, l_fFactor + (1 * 0.001) + 0.0005 );\n" )
					cuT(	"    l_lightReturn.m_v4Specular      = <texture1D>( c3d_sLights, l_fFactor + (2 * 0.001) + 0.0005 );\n" )
					cuT(	"    vec4    l_v4Position            = <texture1D>( c3d_sLights, l_fFactor + (3 * 0.001) + 0.0005 );\n" )
					cuT(	"    l_lightReturn.m_v3Attenuation   = <texture1D>( c3d_sLights, l_fFactor + (4 * 0.001) + 0.0005 ).xyz;\n" )
					cuT(	"    vec4    l_v4A                   = <texture1D>( c3d_sLights, l_fFactor + (5 * 0.001) + 0.0005 );\n" )
					cuT(	"    vec4    l_v4B                   = <texture1D>( c3d_sLights, l_fFactor + (6 * 0.001) + 0.0005 );\n" )
					cuT(	"    vec4    l_v4C                   = <texture1D>( c3d_sLights, l_fFactor + (7 * 0.001) + 0.0005 );\n" )
					cuT(	"    vec4    l_v4D                   = <texture1D>( c3d_sLights, l_fFactor + (8 * 0.001) + 0.0005 );\n" )
					cuT(	"    vec2    l_v2Spot                = <texture1D>( c3d_sLights, l_fFactor + (9 * 0.001) + 0.0005 ).xy;\n" )
					cuT(	"    l_lightReturn.m_v4Position      = vec4( l_v4Position.z, l_v4Position.y, l_v4Position.x, 0.0 );\n" )
					cuT(	"    l_lightReturn.m_iType           = int( l_v4Position.w );\n" )
					cuT(	"    l_lightReturn.m_mtx4Orientation = mat4( l_v4A, l_v4B, l_v4C, l_v4D );\n" )
					cuT(	"    l_lightReturn.m_fExponent       = l_v2Spot.x;\n" )
					cuT(	"    l_lightReturn.m_fCutOff         = l_v2Spot.x;\n" )
					cuT(	"    return l_lightReturn;\n" )
					cuT(	"}\n" )
					// Computes BlinnPhong components : direction and attenuation
					cuT(	"vec4 BlinnPhongDir( in Light p_light, in vec3 p_vertex, in mat4 p_mtxModelView )\n" )
					cuT(	"{\n" )
					cuT(	"    vec4 l_v4Return;\n" )
					cuT(	"    if( 0 != p_light.m_iType ) // non directional light?\n" )
					cuT(	"    {\n" )
					cuT(	"        vec3 l_v3ToLight = p_light.m_v4Position.xyz - p_vertex;\n" )
					cuT(	"        float l_fDistance = length( l_v3ToLight );\n" )
					cuT(	"        l_v4Return.xyz = normalize( l_v3ToLight );\n" )
					cuT(	"        float l_fAtt = p_light.m_v3Attenuation.x\n" )
					cuT(	"                     + p_light.m_v3Attenuation.y * l_fDistance\n" )
					cuT(	"                     + p_light.m_v3Attenuation.z * l_fDistance * l_fDistance;\n" )
					cuT(	"        l_v4Return.w = 1.0 / l_fAtt;\n" )
					cuT(	"        if ( p_light.m_fCutOff <= 90.0 ) // spotlight?\n" )
					cuT(	"        {\n" )
					cuT(	"            float l_clampedCosine = max( 0.0, dot( -l_v4Return.xyz, ( vec4( 0, 0, 1, 0 ) * p_light.m_mtx4Orientation ).xyz ) );\n" )
					cuT(	"            if ( l_clampedCosine < cos( radians( p_light.m_fCutOff ) ) ) // outside of spotlight cone?\n" )
					cuT(	"            {\n" )
					cuT(	"                l_v4Return.w = 0.0;\n" )
					cuT(	"            }\n" )
					cuT(	"            else\n" )
					cuT(	"            {\n" )
					cuT(	"                l_v4Return.w = l_v4Return.w * pow( l_clampedCosine, p_light.m_fExponent );\n" )
					cuT(	"            }\n" )
					cuT(	"        }\n" )
					cuT(	"    }\n" )
					cuT(	"    else\n" )
					cuT(	"    {\n" )
					cuT(	"        l_v4Return = vec4( normalize( vec3( p_light.m_v4Position ) ), 1.0 );\n" )
					cuT(	"    }\n" )
					cuT(	"    return l_v4Return;\n" )
					cuT(	"}\n" )
					// Computes Fresnel component
					cuT(	"float Fresnel( in float p_lambert, in vec3 p_lightDir, in vec3 p_normal, in vec3 p_eye, in float p_shininess, inout vec4 p_specular )\n" )
					cuT(	"{\n" )
					cuT(	"    float l_fFresnel;\n" )
					cuT(	"    if( p_lambert <= 0.0 ) // light source on the wrong side?\n" )
					cuT(	"    {\n" )
					cuT(	"        l_fFresnel = 0.0;\n" )
					cuT(	"    }\n" )
					cuT(	"    else // light source on the right side\n" )
					cuT(	"    {\n" )
					cuT(	"        vec3 l_v3LightReflect = reflect( p_lightDir, p_normal );\n" )
					cuT(	"        l_fFresnel = pow( 1.0 - clamp( dot( normalize( p_lightDir + p_eye ), p_eye ), 0.0, 1.0 ), 5.0 );\n" )
					cuT(	"        p_specular = vec4( clamp( mix( vec3( c3d_v4MatSpecular ), vec3( 1.0 ), l_fFresnel ), 0.0, 1.0 ), 1.0 );\n" )
					cuT(	"        l_fFresnel = pow( clamp( dot( p_eye, l_v3LightReflect ), 0.0, 1.0 ), p_shininess );\n" )
					//cuT(	"        l_fFresnel = pow( max( 0.0, dot( reflect( p_lightDir, p_normal ), p_eye ) ), p_shininess );\n" )
					cuT(	"    }\n" )
					cuT(	"    return l_fFresnel;\n" )
					cuT(	"}\n" )
					// Computes normal mapping components
					cuT(	"void Bump( in vec3 p_v3T, in vec3 p_v3B, in vec3 p_v3N, inout vec3 p_lightDir, inout float p_fAttenuation )\n" )
					cuT(	"{\n" )
					cuT(	"   float l_fInvRadius = 0.02;\n" )
					cuT(	"   p_lightDir = vec3( dot( p_lightDir, p_v3T ), dot( p_lightDir, p_v3B ), dot( p_lightDir, p_v3N ) );\n" )
					cuT(	"   float l_fSqrLength = dot( p_lightDir, p_lightDir );\n" )
					cuT(	"   p_lightDir = p_lightDir * inversesqrt( l_fSqrLength );\n" )
					cuT(	"   p_fAttenuation *= clamp( 1.0 - l_fInvRadius * sqrt( l_fSqrLength ), 0.0, 1.0 );\n" )
					cuT(	"}\n" );
			}

			inline Castor::String const & GetPixelLights()const
			{
				return m_strPixelLights;
			}
		};
	}
}

#endif