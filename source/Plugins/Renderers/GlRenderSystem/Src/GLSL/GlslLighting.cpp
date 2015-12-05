#include "GlslLighting.hpp"

namespace GlRender
{
	namespace GLSL
	{
		namespace BlinnPhong
		{
			Vec4 ComputeLightDirection( InParam< Light > const & p_light, InParam< Vec3 > const & p_position, InParam< Mat4 > const & p_mtxModelView )
			{
				GlslWriter * l_pWriter = p_light.m_writer;
				GlslWriter & l_writer = *l_pWriter;
				LOCALE( l_writer, Vec4, l_v4Return );

				IF( l_writer, p_light.m_iType() != Int( 0 ) )
				{
					LOCALE_ASSIGN( l_writer, Vec3, l_direction, p_position - p_light.m_v4Position().XYZ );
					LOCALE_ASSIGN( l_writer, Float, l_distance, length( l_direction ) );
					l_v4Return.XYZ = normalize( l_direction );
					LOCALE_ASSIGN( l_writer, Float, l_attenuation, p_light.m_v3Attenuation().X
								   + p_light.m_v3Attenuation().Y * l_distance
								   + p_light.m_v3Attenuation().Z * l_distance * l_distance );
					l_v4Return.W = l_attenuation;

					IF( l_writer, p_light.m_fCutOff() <= Float( 90.0f ) ) // spotlight?
					{
						LOCALE( l_writer, Float, l_clampedCosine ) = max( Float( 0.0f ), dot( neg( l_v4Return.XYZ ), l_writer.Paren( vec4( Float( 0.0f ), 0.0f, 1.0f, 0.0f ) * p_light.m_mtx4Orientation() ).XYZ ) );

						IF( l_writer, l_clampedCosine < cos( radians( p_light.m_fCutOff() ) ) )
						{
							l_v4Return.W = Float( 0.0f );
						}
						ELSE
						{
							l_v4Return.W = l_v4Return.W * pow( l_clampedCosine, p_light.m_fExponent() );
						}
						FI
					}
					FI
				}
				ELSE
				{
					l_v4Return = vec4( p_light.m_v4Position().XYZ, 1.0 );
				}
				FI

				l_writer.Return( l_v4Return );
				return l_v4Return;
			}

			Void Bump( InParam< Vec3 > const & p_v3T, InParam< Vec3 > const & p_v3B, InParam< Vec3 > const & p_v3N, InOutParam< Vec3 > & p_lightDir, InOutParam< Float > & p_fAttenuation )
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

			Float ComputeFresnel( InParam< Float > const & p_lambert, InParam< Vec3 > const & p_direction, InParam< Vec3 > const & p_normal, InParam< Vec3 > const & p_eye, InParam< Float > const & p_shininess, InOutParam< Vec3 > & p_specular )
			{
				GlslWriter * l_pWriter = p_lambert.m_writer;
				GlslWriter & l_writer = *l_pWriter;
				LOCALE_ASSIGN( l_writer, Vec3, l_lightReflect, normalize( reflect( p_direction, p_normal ) ) );
				LOCALE_ASSIGN( l_writer, Float, l_fresnel, dot( p_eye, l_lightReflect ) );
				l_fresnel = pow( l_fresnel, p_shininess );
				//Float l_fresnel = pow( 1.0 - clamp( dot( normalize( p_direction + p_eye ), p_eye ), 0.0, 1.0 ), 5.0 );
				//p_specular = clamp( mix( vec3( c3d_v4MatSpecular ), vec3( 1.0 ), l_fresnel ), 0.0, 1.0 );
				//l_fresnel = pow( clamp( dot( p_eye, l_lightReflect ), 0.0, 1.0 ), p_shininess );
				////l_fFresnel = pow( max( 0.0, dot( l_lightReflect, p_eye ) ), p_shininess );
				l_writer.Return( l_fresnel );
				return l_fresnel;
			}
		}

		void BlinnPhongLightingModel::Declare_ComputeLightDirection( GlslWriter & p_writer )
		{
			p_writer.ImplementFunction< Vec4 >( cuT( "ComputeLightDirection" ), &BlinnPhong::ComputeLightDirection,
												InParam< Light >( &p_writer, cuT( "p_light" ) ),
												InParam< Vec3 >( &p_writer, cuT( "p_position" ) ),
												InParam< Mat4 >( &p_writer, cuT( "p_mtxModelView" ) ) );
		}

		void BlinnPhongLightingModel::Declare_Bump( GlslWriter & p_writer )
		{
			InOutParam< Vec3 > p_lightDir( &p_writer, cuT( "p_lightDir" ) );
			InOutParam< Float > p_fAttenuation( &p_writer, cuT( "p_fAttenuation" ) );
			p_writer.ImplementFunction< Void >( cuT( "Bump" ), &BlinnPhong::Bump,
												InParam< Vec3 >( &p_writer, cuT( "p_v3T" ) ),
												InParam< Vec3 >( &p_writer, cuT( "p_v3B" ) ),
												InParam< Vec3 >( &p_writer, cuT( "p_v3N" ) ),
												p_lightDir,
												p_fAttenuation );
		}

		void BlinnPhongLightingModel::Declare_ComputeFresnel( GlslWriter & p_writer )
		{
			InOutParam< Vec3 > p_specular( &p_writer, cuT( "p_specular" ) );
			p_writer.ImplementFunction< Float >( cuT( "ComputeFresnel" ), &BlinnPhong::ComputeFresnel,
												 InParam< Float >( &p_writer, cuT( "p_lambert" ) ),
												 InParam< Vec3 >( &p_writer, cuT( "p_direction" ) ),
												 InParam< Vec3 >( &p_writer, cuT( "p_normal" ) ),
												 InParam< Vec3 >( &p_writer, cuT( "p_eye" ) ),
												 InParam< Float >( &p_writer, cuT( "p_shininess" ) ),
												 p_specular );
		}
	}
}
