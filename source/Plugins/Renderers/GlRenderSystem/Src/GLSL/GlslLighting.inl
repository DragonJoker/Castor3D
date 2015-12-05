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
namespace GlRender
{
	namespace GLSL
	{
		template< typename LightingModel >
		void Lighting< LightingModel >::Declare_Light( GlslWriter & p_writer )
		{
			Struct l_lightDecl = p_writer.GetStruct( cuT( "Light" ) );
			l_lightDecl.GetMember< Vec4 >( cuT( "m_v4Ambient" ) );
			l_lightDecl.GetMember< Vec4 >( cuT( "m_v4Diffuse" ) );
			l_lightDecl.GetMember< Vec4 >( cuT( "m_v4Specular" ) );
			l_lightDecl.GetMember< Vec4 >( cuT( "m_v4Position" ) );
			l_lightDecl.GetMember< Int >( cuT( "m_iType" ) );
			l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Attenuation" ) );
			l_lightDecl.GetMember< Mat4 >( cuT( "m_mtx4Orientation" ) );
			l_lightDecl.GetMember< Float >( cuT( "m_fExponent" ) );
			l_lightDecl.GetMember< Float >( cuT( "m_fCutOff" ) );
			l_lightDecl.End();
		}

		namespace
		{
			Light GetLight( Int p_iIndex )
			{
				BUILTIN( *p_iIndex.m_writer, Sampler1D, c3d_sLights );
				LOCALE( *p_iIndex.m_writer, Light, l_lightReturn );

				if ( p_iIndex.m_writer->HasTexelFetch() )
				{
					LOCALE_ASSIGN( *p_iIndex.m_writer, Int, l_offset, p_iIndex * Int( 10 ) );
					l_lightReturn.m_v4Ambient() = texelFetch( c3d_sLights, l_offset++, 0 );
					//l_lightReturn.m_v4Diffuse() = vec4( Float( p_iIndex.m_writer, 1.0f ), 1.0, 1.0, 1.0 );
					l_lightReturn.m_v4Diffuse() = texelFetch( c3d_sLights, l_offset++, 0 );
					//l_lightReturn.m_v4Specular() = vec4( Float( p_iIndex.m_writer, 1.0f ), 1.0, 1.0, 1.0 );
					l_lightReturn.m_v4Specular() = texelFetch( c3d_sLights, l_offset++, 0 );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4Position, texelFetch( c3d_sLights, l_offset++, 0 ) );
					l_lightReturn.m_v3Attenuation() = texelFetch( c3d_sLights, l_offset++, 0 ).XYZ;
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4A, texelFetch( c3d_sLights, l_offset++, 0 ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4B, texelFetch( c3d_sLights, l_offset++, 0 ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4C, texelFetch( c3d_sLights, l_offset++, 0 ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4D, texelFetch( c3d_sLights, l_offset++, 0 ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec2, l_v2Spot, texelFetch( c3d_sLights, l_offset++, 0 ).XY );
					//l_lightReturn.m_v4Position() = vec4( Float( p_iIndex.m_writer, 0.0f ), -1.0, -1.0, 0.0 );
					l_lightReturn.m_v4Position() = vec4( l_v4Position.Z, l_v4Position.Y, l_v4Position.X, 0.0 );
					l_lightReturn.m_iType() = CAST( *p_iIndex.m_writer, Int, l_v4Position.W );
					l_lightReturn.m_mtx4Orientation() = mat4( l_v4A, l_v4B, l_v4C, l_v4D );
					//l_lightReturn.m_fExponent() = 50.0f;
					l_lightReturn.m_fExponent() = l_v2Spot.X;
					l_lightReturn.m_fCutOff() = l_v2Spot.Y;
				}
				else
				{
					LOCALE_ASSIGN( *p_iIndex.m_writer, Float, l_fFactor, p_iIndex * Float( 0.01f ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Float, l_fOffset, Float( 0 ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Float, l_fDecal, Float( 0.0005f ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Float, l_fMult, Float( 0.001f ) );
					l_lightReturn.m_v4Ambient() = texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal );
					l_fOffset += l_fMult;
					l_lightReturn.m_v4Diffuse() = texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal );
					l_fOffset += l_fMult;
					l_lightReturn.m_v4Specular() = texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal );
					l_fOffset += l_fMult;
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4Position, texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ) );
					l_fOffset += l_fMult;
					l_lightReturn.m_v3Attenuation() = texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ).XYZ;
					l_fOffset += l_fMult;
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4A, texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ) );
					l_fOffset += l_fMult;
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4B, texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ) );
					l_fOffset += l_fMult;
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4C, texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ) );
					l_fOffset += l_fMult;
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4D, texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ) );
					l_fOffset += l_fMult;
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec2, l_v2Spot, texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ).XY );
					l_lightReturn.m_v4Position() = vec4( l_v4Position.Z, l_v4Position.Y, l_v4Position.X, 0.0 );
					l_lightReturn.m_iType() = CAST( *p_iIndex.m_writer, Int, l_v4Position.W );
					l_lightReturn.m_mtx4Orientation() = mat4( l_v4A, l_v4B, l_v4C, l_v4D );
					l_lightReturn.m_fExponent() = l_v2Spot.X;
					l_lightReturn.m_fCutOff() = l_v2Spot.Y;
				}

				p_iIndex.m_writer->Return( l_lightReturn );
				return l_lightReturn;
			}
		}

		template< typename LightingModel >
		void Lighting< LightingModel >::Declare_GetLight( GlslWriter & p_writer )
		{
			p_writer.ImplementFunction< Light >( cuT( "GetLight" ), &GLSL::GetLight, Int( &p_writer, cuT( "p_iIndex" ) ) );
		}

		template< typename LightingModel >
		void Lighting< LightingModel >::Declare_ComputeLightDirection( GlslWriter & p_writer )
		{
			LightingModel::Declare_ComputeLightDirection( p_writer );
		}

		template< typename LightingModel >
		void Lighting< LightingModel >::Declare_Bump( GlslWriter & p_writer )
		{
			LightingModel::Declare_Bump( p_writer );
		}

		template< typename LightingModel >
		void Lighting< LightingModel >::Declare_ComputeFresnel( GlslWriter & p_writer )
		{
			LightingModel::Declare_ComputeFresnel( p_writer );
		}

		template< typename LightingModel >
		Light Lighting< LightingModel >::GetLight( Type const & p_value )
		{
			return WriteFunctionCall< Light >( p_value.m_writer, cuT( "GetLight" ), p_value );
		}

		template< typename LightingModel >
		template< typename ... Values >
		Vec4 Lighting< LightingModel >::ComputeLightDirection( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Vec4 >( p_value.m_writer, cuT( "ComputeLightDirection" ), p_value, p_values... );
		}

		template< typename LightingModel >
		template< typename ... Values >
		void Lighting< LightingModel >::Bump( Type const & p_value, Values const & ... p_values )
		{
			WriteFunctionCall< Void >( p_value.m_writer, cuT( "Bump" ), p_value, p_values... );
		}

		template< typename LightingModel >
		template< typename ... Values >
		Float Lighting< LightingModel >::ComputeFresnel( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "ComputeFresnel" ), p_value, p_values... );
		}
	}
}
