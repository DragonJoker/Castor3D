#include "GlslLighting.hpp"

namespace GlRender
{
	namespace GLSL
	{
		//***********************************************************************************************

		namespace
		{
			Light GetLight( Int p_iIndex )
			{
				LOCALE( *p_iIndex.m_writer, Light, l_lightReturn );

				if ( p_iIndex.m_writer->HasTexelFetch() )
				{
					if ( p_iIndex.m_writer->GetOpenGl().HasTbo() )
					{
						BUILTIN( *p_iIndex.m_writer, SamplerBuffer, c3d_sLights );
						LOCALE_ASSIGN( *p_iIndex.m_writer, Int, l_offset, p_iIndex * Int( 10 ) );
						l_lightReturn.m_v4Ambient() = texelFetch( c3d_sLights, l_offset++ );
						l_lightReturn.m_v4Diffuse() = texelFetch( c3d_sLights, l_offset++ );
						l_lightReturn.m_v4Specular() = texelFetch( c3d_sLights, l_offset++ );
						LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4Position, texelFetch( c3d_sLights, l_offset++ ) );
						l_lightReturn.m_v3Attenuation() = texelFetch( c3d_sLights, l_offset++ ).XYZ;
						LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4A, texelFetch( c3d_sLights, l_offset++ ) );
						LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4B, texelFetch( c3d_sLights, l_offset++ ) );
						LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4C, texelFetch( c3d_sLights, l_offset++ ) );
						LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4D, texelFetch( c3d_sLights, l_offset++ ) );
						LOCALE_ASSIGN( *p_iIndex.m_writer, Vec2, l_v2Spot, texelFetch( c3d_sLights, l_offset++ ).XY );
						l_lightReturn.m_v4Position() = vec4( l_v4Position.Z, l_v4Position.Y, l_v4Position.X, 0.0 );
						l_lightReturn.m_iType() = CAST( *p_iIndex.m_writer, Int, l_v4Position.W );
						l_lightReturn.m_v3Direction() = p_iIndex.m_writer->Paren( mat4( l_v4A, l_v4B, l_v4C, l_v4D ) * vec4( Float( p_iIndex.m_writer, 0.0f ), 0.0f, 1.0f, 0.0f ) ).XYZ;
						l_lightReturn.m_fExponent() = l_v2Spot.X;
						l_lightReturn.m_fCutOff() = l_v2Spot.Y;
					}
					else
					{
						BUILTIN( *p_iIndex.m_writer, Sampler1D, c3d_sLights );
						LOCALE_ASSIGN( *p_iIndex.m_writer, Int, l_offset, p_iIndex * Int( 10 ) );
						l_lightReturn.m_v4Ambient() = texelFetch( c3d_sLights, l_offset++, 0 );
						l_lightReturn.m_v4Diffuse() = texelFetch( c3d_sLights, l_offset++, 0 );
						l_lightReturn.m_v4Specular() = texelFetch( c3d_sLights, l_offset++, 0 );
						LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4Position, texelFetch( c3d_sLights, l_offset++, 0 ) );
						l_lightReturn.m_v3Attenuation() = texelFetch( c3d_sLights, l_offset++, 0 ).XYZ;
						LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4A, texelFetch( c3d_sLights, l_offset++, 0 ) );
						LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4B, texelFetch( c3d_sLights, l_offset++, 0 ) );
						LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4C, texelFetch( c3d_sLights, l_offset++, 0 ) );
						LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4D, texelFetch( c3d_sLights, l_offset++, 0 ) );
						LOCALE_ASSIGN( *p_iIndex.m_writer, Vec2, l_v2Spot, texelFetch( c3d_sLights, l_offset++, 0 ).XY );
						l_lightReturn.m_v4Position() = vec4( l_v4Position.Z, l_v4Position.Y, l_v4Position.X, 0.0 );
						l_lightReturn.m_iType() = CAST( *p_iIndex.m_writer, Int, l_v4Position.W );
						l_lightReturn.m_v3Direction() = p_iIndex.m_writer->Paren( mat4( l_v4A, l_v4B, l_v4C, l_v4D ) * vec4( Float( p_iIndex.m_writer, 0.0f ), 0.0f, 1.0f, 0.0f ) ).XYZ;
						l_lightReturn.m_fExponent() = l_v2Spot.X;
						l_lightReturn.m_fCutOff() = l_v2Spot.Y;
					}
				}
				else
				{
					BUILTIN( *p_iIndex.m_writer, Sampler1D, c3d_sLights );
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
					l_lightReturn.m_v3Direction() = p_iIndex.m_writer->Paren( mat4( l_v4A, l_v4B, l_v4C, l_v4D ) * vec4( Float( p_iIndex.m_writer, 0.0f ), 0.0f, 1.0f, 0.0f ) ).XYZ;
					l_lightReturn.m_fExponent() = l_v2Spot.X;
					l_lightReturn.m_fCutOff() = l_v2Spot.Y;
				}

				p_iIndex.m_writer->Return( l_lightReturn );
				return l_lightReturn;
			}
		}

		//***********************************************************************************************

		void LightingModel::Declare_Light( GlslWriter & p_writer )
		{
			Struct l_lightDecl = p_writer.GetStruct( cuT( "Light" ) );
			l_lightDecl.GetMember< Vec4 >( cuT( "m_v4Ambient" ) );
			l_lightDecl.GetMember< Vec4 >( cuT( "m_v4Diffuse" ) );
			l_lightDecl.GetMember< Vec4 >( cuT( "m_v4Specular" ) );
			l_lightDecl.GetMember< Vec4 >( cuT( "m_v4Position" ) );
			l_lightDecl.GetMember< Int >( cuT( "m_iType" ) );
			l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Attenuation" ) );
			l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Direction" ) );
			l_lightDecl.GetMember< Float >( cuT( "m_fExponent" ) );
			l_lightDecl.GetMember< Float >( cuT( "m_fCutOff" ) );
			l_lightDecl.End();
		}

		void LightingModel::Declare_GetLight( GlslWriter & p_writer )
		{
			p_writer.ImplementFunction< Light >( cuT( "GetLight" ), &GLSL::GetLight, Int( &p_writer, cuT( "p_iIndex" ) ) );
		}

		Light LightingModel::GetLight( Type const & p_value )
		{
			return WriteFunctionCall< Light >( p_value.m_writer, cuT( "GetLight" ), p_value );
		}

		//***********************************************************************************************

		namespace
		{
			Vec3 Bump( Vec3 & p_v3T, Vec3 & p_v3B, Vec3 & p_v3N, Vec3 & p_v3MapMormal )
			{
				GlslWriter * l_pWriter = p_v3T.m_writer;
				GlslWriter & l_writer = *l_pWriter;
				return normalize( mat3( p_v3T, p_v3B, p_v3N ) * p_v3MapMormal );
			}
		}

		void BlinnPhongLightingModel::WriteCompute( uint64_t p_flags, GlslWriter & p_writer, Int const & i,
													Vec3 & p_v3MapSpecular, Mat4 & c3d_mtxModelView, Vec3 & p_v3MapMormal,
													Vec4 & c3d_v4MatAmbient, Vec4 & c3d_v4MatDiffuse, Vec4 & c3d_v4MatSpecular,
													Vec3 & p_v3Position, Vec3 & p_v3Normal, Vec3 & p_v3Eye, Float & p_fShininess,
													Vec3 & p_vtxVertex, Vec3 & p_vtxTangent, Vec3 & p_vtxBitangent, Vec3 & p_vtxNormal,
													Vec3 & p_v3Ambient, Vec3 & p_v3Diffuse, Vec3 & p_v3Specular )
		{
			if ( ( p_flags & Castor3D::eTEXTURE_CHANNEL_NORMAL ) == Castor3D::eTEXTURE_CHANNEL_NORMAL )
			{
				p_v3Normal = Bump( p_vtxTangent, p_vtxBitangent, p_vtxNormal, p_v3MapMormal );
			}

			LOCALE_ASSIGN( p_writer, GLSL::Light, l_light, GetLight( i ) );
			LOCALE_ASSIGN( p_writer, Vec3, l_lightDirection, l_light.m_v4Position().XYZ );
			LOCALE_ASSIGN( p_writer, Float, l_distance, Float( 0.0f ) );
			LOCALE_ASSIGN( p_writer, Float, l_spotFactor, Float( 1.0f ) );
			LOCALE_ASSIGN( p_writer, Float, l_cutoff, Float( 0.0f ) );

			IF ( p_writer, l_light.m_v4Position ().W != Float( 0.0f ) )
			{
				IF ( p_writer, l_light.m_v4Position ().W == Float( 2.0f ) )
				{
					LOCALE_ASSIGN( p_writer, Vec3, l_lightToPixel, p_v3Position - l_light.m_v4Position().XYZ );
					l_spotFactor = dot( l_lightToPixel, l_light.m_v3Direction() );
					l_cutoff = l_light.m_fCutOff();
				}
				FI

				l_lightDirection = p_v3Position - l_lightDirection;
				l_distance = length( l_lightDirection );
			}
			FI

			IF ( p_writer, l_spotFactor > l_cutoff )
			{
				LOCALE_ASSIGN( p_writer, Vec3, l_v3TmpAmbient, c3d_v4MatAmbient.XYZ * ( l_light.m_v4Ambient().XYZ * l_light.m_v4Ambient().W ) );
				LOCALE_ASSIGN( p_writer, Float, l_diffuseFactor, dot( p_v3Normal, neg( l_lightDirection ) ) );
				LOCALE_ASSIGN( p_writer, Vec3, l_v3TmpDiffuse, vec3( Float( &p_writer, 0.0f ), 0.0f, 0.0f ) );
				LOCALE_ASSIGN( p_writer, Vec3, l_v3TmpSpecular, vec3( Float( &p_writer, 0.0f ), 0.0f, 0.0f ) );

				IF( p_writer, l_diffuseFactor > Float( 0.0f ) )
				{
					l_v3TmpDiffuse = vec3( l_light.m_v4Diffuse().XYZ * l_light.m_v4Diffuse().W * l_diffuseFactor );
					LOCALE_ASSIGN( p_writer, Vec3, l_vertexToEye, normalize( p_v3Eye - p_v3Position ) );
					LOCALE_ASSIGN( p_writer, Vec3, l_lightReflect, normalize( reflect( l_light.m_v4Position().XYZ, p_v3Normal ) ) );
					LOCALE_ASSIGN( p_writer, Float, l_specularFactor, dot( l_vertexToEye, l_lightReflect ) );

					IF( p_writer, l_specularFactor > Float( 0.0f ) )
					{
						l_specularFactor = pow( l_specularFactor, p_fShininess );
						l_v3TmpSpecular = l_light.m_v4Specular().XYZ * l_light.m_v4Specular().W * l_specularFactor;
					}
					FI
				}
				FI

				IF ( p_writer, l_light.m_v4Position ().W != Float( 0.0f ) )
				{
					LOCALE_ASSIGN( p_writer, Float, l_attenuation, l_light.m_v3Attenuation().X + l_light.m_v3Attenuation().Y * l_distance + l_light.m_v3Attenuation().Z * l_distance * l_distance );
					l_v3TmpAmbient /= l_attenuation;
					l_v3TmpDiffuse /= l_attenuation;
					l_v3TmpSpecular /= l_attenuation;

					IF ( p_writer, l_light.m_v4Position ().W == Float( 2.0f ) )
					{
						l_spotFactor = ( 1.0f - p_writer.Paren( 1.0f - l_spotFactor ) * 1.0f / p_writer.Paren( 1.0f - l_cutoff ) );
						l_v3TmpAmbient *= l_spotFactor;
						l_v3TmpDiffuse *= l_spotFactor;
						l_v3TmpSpecular *= l_spotFactor;
					}
					FI
				}
				FI

				p_v3Ambient += l_v3TmpAmbient;
				p_v3Diffuse += l_v3TmpDiffuse;
				p_v3Specular += l_v3TmpSpecular;
			}
			FI
		}

		//***********************************************************************************************
	}
}
