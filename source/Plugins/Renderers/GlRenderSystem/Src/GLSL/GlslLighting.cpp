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
				BUILTIN( *p_iIndex.m_writer, Sampler1D, c3d_sLights );
				LOCALE( *p_iIndex.m_writer, Light, l_lightReturn );

				if ( false )//p_iIndex.m_writer->HasTexelFetch() )
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
			l_lightDecl.GetMember< Mat4 >( cuT( "m_mtx4Orientation" ) );
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

		void BlinnPhongLightingModel::WriteCompute( uint64_t p_flags, GlslWriter & p_writer, Int & i,
													Vec3 & p_v3MapSpecular, Mat4 c3d_mtxModelView,
													Vec4 & c3d_v4MatAmbient, Vec4 & c3d_v4MatDiffuse, Vec4 & c3d_v4MatSpecular,
													Vec3 & p_v3Normal, Vec3 & p_v3EyeVec, Float & p_fShininess,
													Vec3 & p_vtxVertex, Vec3 & p_vtxTangent, Vec3 & p_vtxBitangent, Vec3 & p_vtxNormal,
													Vec3 & p_v3Ambient, Vec3 & p_v3Diffuse, Vec3 & p_v3Specular )
		{
			LOCALE_ASSIGN( p_writer, GLSL::Light, l_light, GetLight( i ) );
			Vec4 l_v4LightDir( DoComputeLightDirection( l_light, p_vtxVertex, c3d_mtxModelView ) );
			LOCALE_ASSIGN( p_writer, Vec3, l_v3LightDir, l_v4LightDir.XYZ );
			LOCALE_ASSIGN( p_writer, Float, l_fAttenuation, l_v4LightDir.W );

			if ( ( p_flags & Castor3D::eTEXTURE_CHANNEL_NORMAL ) == Castor3D::eTEXTURE_CHANNEL_NORMAL )
			{
				DoBump( p_vtxTangent, p_vtxBitangent, p_vtxNormal, l_v3LightDir, l_fAttenuation );
			}

			LOCALE_ASSIGN( p_writer, Float, l_fLambert, max( dot( p_v3Normal, l_v3LightDir ), 0.0 ) );
			//l_fLambert = dot( l_v3Normal, -l_v3LightDir );
			LOCALE_ASSIGN( p_writer, Vec3, l_v3MatSpecular, c3d_v4MatSpecular.XYZ );
			Float l_fFresnel( DoComputeFresnel( l_fLambert, l_v3LightDir, p_v3Normal, p_v3EyeVec, p_fShininess, l_v3MatSpecular ) );
			LOCALE_ASSIGN( p_writer, Vec3, l_v3TmpAmbient, ( ( l_light.m_v4Ambient().XYZ * l_light.m_v4Ambient().W ) * c3d_v4MatAmbient.XYZ ) );
			LOCALE_ASSIGN( p_writer, Vec3, l_v3TmpDiffuse, ( l_light.m_v4Diffuse().XYZ * l_light.m_v4Diffuse().W * c3d_v4MatDiffuse.XYZ * l_fLambert ) / l_fAttenuation );
			LOCALE_ASSIGN( p_writer, Vec3, l_v3TmpSpecular, ( l_light.m_v4Specular().XYZ * l_light.m_v4Specular().W * l_v3MatSpecular * l_fFresnel ) / l_fAttenuation );

			if ( ( p_flags & Castor3D::eTEXTURE_CHANNEL_SPECULAR ) == Castor3D::eTEXTURE_CHANNEL_SPECULAR )
			{
				l_v3TmpSpecular = l_fAttenuation * l_light.m_v4Specular().XYZ * p_v3MapSpecular * l_v3TmpSpecular;
			}

			p_v3Ambient += l_v3TmpAmbient;
			p_v3Diffuse += l_v3TmpDiffuse;
			p_v3Specular += l_v3TmpSpecular;
		}

		Vec4 BlinnPhongLightingModel::DoComputeLightDirection( Light & p_light, Vec3 & p_position, Mat4 & p_mtxModelView )
		{
			GlslWriter * l_pWriter = p_light.m_writer;
			GlslWriter & l_writer = *l_pWriter;
			LOCALE( l_writer, Vec4, l_v4LightDir );

			IF( l_writer, p_light.m_iType() != Int( 0 ) )
			{
				LOCALE_ASSIGN( l_writer, Vec3, l_direction, p_position - p_light.m_v4Position().XYZ );
				LOCALE_ASSIGN( l_writer, Float, l_distance, length( l_direction ) );
				l_v4LightDir.XYZ = normalize( l_direction );
				LOCALE_ASSIGN( l_writer, Float, l_attenuation, p_light.m_v3Attenuation().X
							   + p_light.m_v3Attenuation().Y * l_distance
							   + p_light.m_v3Attenuation().Z * l_distance * l_distance );
				l_v4LightDir.W = l_attenuation;

				IF( l_writer, p_light.m_fCutOff() <= Float( 90.0f ) ) // spotlight?
				{
					LOCALE( l_writer, Float, l_clampedCosine ) = max( Float( 0.0f ), dot( neg( l_v4LightDir.XYZ ), l_writer.Paren( vec4( Float( 0.0f ), 0.0f, 1.0f, 0.0f ) * p_light.m_mtx4Orientation() ).XYZ ) );

					IF( l_writer, l_clampedCosine < cos( radians( p_light.m_fCutOff() ) ) )
					{
						l_v4LightDir.W = Float( 0.0f );
					}
					ELSE
					{
						l_v4LightDir.W = l_v4LightDir.W * pow( l_clampedCosine, p_light.m_fExponent() );
					}
					FI
				}
				FI
			}
			ELSE
			{
				l_v4LightDir = vec4( p_light.m_v4Position().XYZ, 1.0 );
			}
			FI

			return l_v4LightDir;
		}

		Void BlinnPhongLightingModel::DoBump( Vec3 & p_v3T, Vec3 & p_v3B, Vec3 & p_v3N, Vec3 & p_lightDir, Float & p_fAttenuation )
		{
			GlslWriter * l_pWriter = p_v3T.m_writer;
			GlslWriter & l_writer = *l_pWriter;
			LOCALE_ASSIGN( l_writer, Float, l_fInvRadius, Float( 0.02f ) );
			p_lightDir = vec3( dot( p_lightDir, p_v3T ), dot( p_lightDir, p_v3B ), dot( p_lightDir, p_v3N ) );
			LOCALE_ASSIGN( l_writer, Float, l_fSqrLength, dot( p_lightDir, p_lightDir ) );
			p_lightDir = p_lightDir * inversesqrt( l_fSqrLength );
			p_fAttenuation *= clamp( Float( 1.0f ) - l_fInvRadius * sqrt( l_fSqrLength ), 0.0, 1.0 );
			return Void();
		}

		Float BlinnPhongLightingModel::DoComputeFresnel( Float & p_lambert, Vec3 & p_direction, Vec3 & p_normal, Vec3 & p_eye, Float & p_shininess, Vec3 & p_specular )
		{
			GlslWriter * l_pWriter = p_lambert.m_writer;
			GlslWriter & l_writer = *l_pWriter;
			LOCALE_ASSIGN( l_writer, Vec3, l_lightReflect, normalize( reflect( p_direction, p_normal ) ) );
			LOCALE_ASSIGN( l_writer, Float, l_fFresnel, dot( p_eye, l_lightReflect ) );
			l_fFresnel = pow( l_fFresnel, p_shininess );
			//l_fFresnel = pow( 1.0 - clamp( dot( normalize( p_direction + p_eye ), p_eye ), 0.0, 1.0 ), 5.0 );
			//p_specular = clamp( mix( vec3( c3d_v4MatSpecular ), vec3( 1.0 ), l_fFresnel ), 0.0, 1.0 );
			//l_fFresnel = pow( clamp( dot( p_eye, l_lightReflect ), 0.0, 1.0 ), p_shininess );
			////l_fFresnel = pow( max( 0.0, dot( l_lightReflect, p_eye ) ), p_shininess );
			return l_fFresnel;
		}

		//***********************************************************************************************
	}
}
