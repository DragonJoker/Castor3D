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

		Castor::String ParamToString( Castor::String & p_sep, FragmentInput const & p_value )
		{
			Castor::StringStream l_return;
			l_return << ParamToString( p_sep, p_value.m_v3Vertex );
			l_return << ParamToString( p_sep, p_value.m_v3Normal );
			l_return << ParamToString( p_sep, p_value.m_v3Tangent );
			l_return << ParamToString( p_sep, p_value.m_v3Bitangent );
			return l_return.str();
		}

		Castor::String ParamToString( Castor::String & p_sep, OutputComponents const & p_value )
		{
			Castor::StringStream l_return;
			l_return << ParamToString( p_sep, p_value.m_v3Ambient );
			l_return << ParamToString( p_sep, p_value.m_v3Diffuse );
			l_return << ParamToString( p_sep, p_value.m_v3Specular );
			return l_return.str();
		}

		Castor::String ToString( FragmentInput const & p_value )
		{
			Castor::StringStream l_return;
			l_return << ToString( p_value.m_v3Vertex ) << ", ";
			l_return << ToString( p_value.m_v3Normal ) << ", ";
			l_return << ToString( p_value.m_v3Tangent ) << ", ";
			l_return << ToString( p_value.m_v3Bitangent );
			return l_return.str();
		}

		Castor::String ToString( OutputComponents const & p_value )
		{
			Castor::StringStream l_return;
			l_return << ToString( p_value.m_v3Ambient ) << ", ";
			l_return << ToString( p_value.m_v3Diffuse ) << ", ";
			l_return << ToString( p_value.m_v3Specular );
			return l_return.str();
		}

		//***********************************************************************************************

		LightingModel::LightingModel( uint32_t p_flags, GlslWriter & p_writer )
			: m_flags( p_flags )
			, m_writer( p_writer )
		{
		}

		void LightingModel::DeclareModel()
		{
			Declare_Light();
			Declare_GetLight();
			DoDeclareModel();
			Declare_ComputeDirectionalLight();
			Declare_ComputePointLight();
			Declare_ComputeSpotLight();
		}

		void LightingModel::Declare_Light()
		{
			Struct l_lightDecl = m_writer.GetStruct( cuT( "Light" ) );
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

		void LightingModel::Declare_GetLight()
		{
			m_writer.ImplementFunction< Light >( cuT( "GetLight" ), &GLSL::GetLight, Int( &m_writer, cuT( "p_iIndex" ) ) );
		}

		Light LightingModel::GetLight( Type const & p_value )
		{
			return WriteFunctionCall< Light >( &m_writer, cuT( "GetLight" ), p_value );
		}

		void LightingModel::ComputeDirectionalLight( Light const & p_light, Vec3 const & p_worldEye, Float const & p_shininess,
													 FragmentInput const & p_fragmentIn, OutputComponents & p_output )
		{
			m_writer << WriteFunctionCall< Void >( &m_writer, cuT( "ComputeDirectionalLight" ), p_light, p_worldEye, p_shininess, p_fragmentIn, p_output ) << Endi();
		}

		void LightingModel::ComputePointLight( Light const & p_light, Vec3 const & p_worldEye, Float const & p_shininess,
											   FragmentInput const & p_fragmentIn, OutputComponents & p_output )
		{
			m_writer << WriteFunctionCall< Void >( &m_writer, cuT( "ComputePointLight" ), p_light, p_worldEye, p_shininess, p_fragmentIn, p_output ) << Endi();
		}

		void LightingModel::ComputeSpotLight( Light const & p_light, Vec3 const & p_worldEye, Float const & p_shininess,
											  FragmentInput const & p_fragmentIn, OutputComponents & p_output )
		{
			m_writer << WriteFunctionCall< Void >( &m_writer, cuT( "ComputeSpotLight" ), p_light, p_worldEye, p_shininess, p_fragmentIn, p_output ) << Endi();
		}

		//***********************************************************************************************

		const Castor::String PhongLightingModel::Name = cuT( "phong" );

		PhongLightingModel::PhongLightingModel( uint32_t p_flags, GlslWriter & p_writer )
			: LightingModel( p_flags, p_writer )
		{
		}

		std::unique_ptr< LightingModel > PhongLightingModel::Create( uint32_t p_flags, GlslWriter & p_writer )
		{
			return std::make_unique< PhongLightingModel >( p_flags, p_writer );
		}

		void PhongLightingModel::DoDeclareModel()
		{
			DoDeclare_ComputeLight();
		}

		void PhongLightingModel::Declare_ComputeDirectionalLight()
		{
			OutputComponents l_output{ InOutParam< Vec3 >( &m_writer, cuT( "p_v3Ambient" ) ), InOutParam< Vec3 >( &m_writer, cuT( "p_v3Diffuse" ) ), InOutParam< Vec3 >( &m_writer, cuT( "p_v3Specular" ) ) };
			m_writer.ImplementFunction< Void >( cuT( "ComputeDirectionalLight" ), [this]( Light const & p_light, Vec3 const & p_worldEye, Float const & p_shininess,
																						  FragmentInput const & p_fragmentIn, OutputComponents & p_output )
			{
				DoComputeLight( p_light, p_worldEye, normalize( -p_light.m_v4Position().ZYX ), p_shininess, p_fragmentIn, p_output );

			}, Light( &m_writer, cuT( "p_light" ) ), InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) ), InParam< Float >( &m_writer, cuT( "p_shininess" ) ),
				FragmentInput{ InParam< Vec3 >( &m_writer, cuT( "p_v3Vertex" ) ), InParam< Vec3 >( &m_writer, cuT( "p_v3Normal" ) ), InParam< Vec3 >( &m_writer, cuT( "p_v3Tangent" ) ), InParam< Vec3 >( &m_writer, cuT( "p_v3Bitangent" ) ) },
				l_output );
		}

		void PhongLightingModel::Declare_ComputePointLight()
		{
			OutputComponents l_output{ InOutParam< Vec3 >( &m_writer, cuT( "p_v3Ambient" ) ), InOutParam< Vec3 >( &m_writer, cuT( "p_v3Diffuse" ) ), InOutParam< Vec3 >( &m_writer, cuT( "p_v3Specular" ) ) };
			m_writer.ImplementFunction< Void >( cuT( "ComputePointLight" ), [this]( Light const & p_light, Vec3 const & p_worldEye, Float const & p_shininess,
																					FragmentInput const & p_fragmentIn, OutputComponents & p_output )
			{
				LOCALE_ASSIGN( m_writer, Vec3, l_lightDirection, p_fragmentIn.m_v3Vertex - p_light.m_v4Position().XYZ );
				LOCALE_ASSIGN( m_writer, Float, l_distance, length( l_lightDirection ) );
				l_lightDirection = normalize( l_lightDirection );
				DoComputeLight( p_light, p_worldEye, l_lightDirection, p_shininess, p_fragmentIn, p_output );

				LOCALE_ASSIGN( m_writer, Float, l_attenuation, p_light.m_v3Attenuation().X + p_light.m_v3Attenuation().Y * l_distance + p_light.m_v3Attenuation().Z * l_distance * l_distance );
				p_output.m_v3Ambient /= l_attenuation;
				p_output.m_v3Diffuse /= l_attenuation;
				p_output.m_v3Specular /= l_attenuation;

			}, Light( &m_writer, cuT( "p_light" ) ), InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) ), InParam< Float >( &m_writer, cuT( "p_shininess" ) ),
				FragmentInput{ InParam< Vec3 >( &m_writer, cuT( "p_v3Vertex" ) ), InParam< Vec3 >( &m_writer, cuT( "p_v3Normal" ) ), InParam< Vec3 >( &m_writer, cuT( "p_v3Tangent" ) ), InParam< Vec3 >( &m_writer, cuT( "p_v3Bitangent" ) ) },
				l_output );
		}

		void PhongLightingModel::Declare_ComputeSpotLight()
		{
			OutputComponents l_output{ InOutParam< Vec3 >( &m_writer, cuT( "p_v3Ambient" ) ), InOutParam< Vec3 >( &m_writer, cuT( "p_v3Diffuse" ) ), InOutParam< Vec3 >( &m_writer, cuT( "p_v3Specular" ) ) };
			m_writer.ImplementFunction< Void >( cuT( "ComputeSpotLight" ), [this]( Light const & p_light, Vec3 const & p_worldEye, Float const & p_shininess,
																				   FragmentInput const & p_fragmentIn, OutputComponents & p_output )
			{
				LOCALE_ASSIGN( m_writer, Vec3, l_lightToPixel, p_fragmentIn.m_v3Vertex - p_light.m_v4Position().XYZ );
				LOCALE_ASSIGN( m_writer, Float, l_spotFactor, dot( l_lightToPixel, p_light.m_v3Direction() ) );

				IF( m_writer, l_spotFactor > p_light.m_fCutOff() )
				{
					ComputePointLight( p_light, p_worldEye, p_shininess, p_fragmentIn, p_output );
					l_spotFactor = m_writer.Paren( Float( 1 ) - m_writer.Paren( Float( 1 ) - l_spotFactor ) * Float( 1 ) / m_writer.Paren( Float( 1 ) - p_light.m_fCutOff() ) );
					p_output.m_v3Ambient *= l_spotFactor;
					p_output.m_v3Diffuse *= l_spotFactor;
					p_output.m_v3Specular *= l_spotFactor;
				}
				FI;

			}, Light( &m_writer, cuT( "p_light" ) ), InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) ), InParam< Float >( &m_writer, cuT( "p_shininess" ) ),
					FragmentInput{ InParam< Vec3 >( &m_writer, cuT( "p_v3Vertex" ) ), InParam< Vec3 >( &m_writer, cuT( "p_v3Normal" ) ), InParam< Vec3 >( &m_writer, cuT( "p_v3Tangent" ) ), InParam< Vec3 >( &m_writer, cuT( "p_v3Bitangent" ) ) },
				l_output );
		}

		void PhongLightingModel::DoDeclare_ComputeLight()
		{
			OutputComponents l_output{ InOutParam< Vec3 >( &m_writer, cuT( "p_v3Ambient" ) ), InOutParam< Vec3 >( &m_writer, cuT( "p_v3Diffuse" ) ), InOutParam< Vec3 >( &m_writer, cuT( "p_v3Specular" ) ) };
			m_writer.ImplementFunction< Void >( cuT( "DoComputeLight" ), [this]( Light const & p_light, Vec3 const & p_worldEye, Vec3 const & p_direction, Float const & p_shininess,
																			   FragmentInput const & p_fragmentIn, OutputComponents & p_output )
			{
				p_output.m_v3Ambient = p_light.m_v4Ambient().XYZ * p_light.m_v4Ambient().W;

				LOCALE_ASSIGN( m_writer, Float, l_diffuseFactor, dot( p_fragmentIn.m_v3Normal, -p_direction ) );

				IF( m_writer, l_diffuseFactor > Float( 0 ) )
				{
					p_output.m_v3Diffuse = p_light.m_v4Diffuse().XYZ * p_light.m_v4Diffuse().W * l_diffuseFactor;

					LOCALE_ASSIGN( m_writer, Vec3, l_vertexToEye, normalize( p_worldEye - p_fragmentIn.m_v3Vertex ) );
					LOCALE_ASSIGN( m_writer, Vec3, l_lightReflect, normalize( reflect( p_direction, p_fragmentIn.m_v3Normal ) ) );
					LOCALE_ASSIGN( m_writer, Float, l_specularFactor, dot( l_vertexToEye, l_lightReflect ) );

					IF( m_writer, l_specularFactor > Float( 0 ) )
					{
						l_specularFactor = pow( l_specularFactor, p_shininess );
						p_output.m_v3Specular = p_light.m_v4Specular().XYZ * p_light.m_v4Specular().W * l_specularFactor;
					}
					FI;
				}
				FI;

			}, InParam< Light >( &m_writer, cuT( "p_light" ) ), InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) ), InParam< Vec3 >( &m_writer, cuT( "p_direction" ) ), InParam< Float >( &m_writer, cuT( "p_shininess" ) ),
				FragmentInput{ InParam< Vec3 >( &m_writer, cuT( "p_v3Vertex" ) ), InParam< Vec3 >( &m_writer, cuT( "p_v3Normal" ) ), InParam< Vec3 >( &m_writer, cuT( "p_v3Tangent" ) ), InParam< Vec3 >( &m_writer, cuT( "p_v3Bitangent" ) ) },
				l_output );
		}

		void PhongLightingModel::DoComputeLight( Light const & p_light, Vec3 const & p_worldEye, Vec3 const & p_direction, Float const & p_shininess,
												 FragmentInput const & p_fragmentIn, OutputComponents & p_output )
		{
			m_writer << WriteFunctionCall< Vec3 >( &m_writer, cuT( "DoComputeLight" ), p_light, p_worldEye, p_direction, p_shininess, p_fragmentIn, p_output ) << Endi();
		}

		//***********************************************************************************************
	}
}
