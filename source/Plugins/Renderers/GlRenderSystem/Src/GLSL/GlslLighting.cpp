#include "GlslLighting.hpp"

namespace GlRender
{
	namespace GLSL
	{
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
			Declare_GetLightColourAndPosition();
			Declare_GetDirectionalLight();
			Declare_GetPointLight();
			Declare_GetSpotLight();
			DoDeclareModel();
			Declare_ComputeDirectionalLight();
			Declare_ComputePointLight();
			Declare_ComputeSpotLight();
		}

		void LightingModel::Declare_Light()
		{
			Struct l_lightDecl = m_writer.GetStruct( cuT( "Light" ) );
			l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Colour" ) );
			l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Intensity" ) );
			l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Position" ) );
			l_lightDecl.GetMember< Int >( cuT( "m_iType" ) );
			l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Attenuation" ) );
			l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Direction" ) );
			l_lightDecl.GetMember< Float >( cuT( "m_fExponent" ) );
			l_lightDecl.GetMember< Float >( cuT( "m_fCutOff" ) );
			l_lightDecl.End();
		}

		void LightingModel::Declare_GetLightColourAndPosition()
		{
			m_writer.ImplementFunction< Light >( cuT( "GetLightColourAndPosition" ), [this]( Int const & p_index )
			{
				LOCALE( m_writer, Light, l_lightReturn );

				if ( m_writer.HasTexelFetch() )
				{
					if ( m_writer.GetOpenGl().HasTbo() )
					{
						BUILTIN( m_writer, SamplerBuffer, c3d_sLights );
						LOCALE_ASSIGN( m_writer, Int, l_offset, p_index * Int( 10 ) );
						l_lightReturn.m_v3Colour() = texelFetch( c3d_sLights, l_offset++ ).RGB;
						l_lightReturn.m_v3Intensity() = texelFetch( c3d_sLights, l_offset++ ).RGB;
						LOCALE_ASSIGN( m_writer, Vec4, l_v4Position, texelFetch( c3d_sLights, l_offset++ ) );
						l_lightReturn.m_v3Position() = l_v4Position.XYZ;
						l_lightReturn.m_iType() = CAST( m_writer, Int, l_v4Position.W );
					}
					else
					{
						BUILTIN( m_writer, Sampler1D, c3d_sLights );
						LOCALE_ASSIGN( m_writer, Int, l_offset, p_index * Int( 10 ) );
						l_lightReturn.m_v3Colour() = texelFetch( c3d_sLights, l_offset++, 0 ).RGB;
						l_lightReturn.m_v3Intensity() = texelFetch( c3d_sLights, l_offset++, 0 ).RGB;
						LOCALE_ASSIGN( m_writer, Vec4, l_v4Position, texelFetch( c3d_sLights, l_offset++, 0 ) );
						l_lightReturn.m_v3Position() = l_v4Position.XYZ;
						l_lightReturn.m_iType() = CAST( m_writer, Int, l_v4Position.W );
					}
				}
				else
				{
					BUILTIN( m_writer, Sampler1D, c3d_sLights );
					LOCALE_ASSIGN( m_writer, Float, l_fFactor, p_index * Float( 0.01f ) );
					LOCALE_ASSIGN( m_writer, Float, l_fOffset, Float( 0 ) );
					LOCALE_ASSIGN( m_writer, Float, l_fDecal, Float( 0.0005f ) );
					LOCALE_ASSIGN( m_writer, Float, l_fMult, Float( 0.001f ) );
					l_lightReturn.m_v3Colour() = texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ).RGB;
					l_fOffset += l_fMult;
					l_lightReturn.m_v3Intensity() = texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ).RGB;
					l_fOffset += l_fMult;
					LOCALE_ASSIGN( m_writer, Vec4, l_v4Position, texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ) );
					l_lightReturn.m_v3Position() = l_v4Position.XYZ;
					l_lightReturn.m_iType() = CAST( m_writer, Int, l_v4Position.W );
				}

				m_writer.Return( l_lightReturn );

			}, Int( &m_writer, cuT( "p_index" ) ) );
		}

		void LightingModel::Declare_GetDirectionalLight()
		{
			m_writer.ImplementFunction< Light >( cuT( "GetDirectionalLight" ), [this]( Int const & p_index )
			{
				LOCALE_ASSIGN( m_writer, Light, l_lightReturn, GetLightColourAndPosition( p_index ) );
				m_writer.Return( l_lightReturn );

			}, Int( &m_writer, cuT( "p_index" ) ) );
		}

		void LightingModel::Declare_GetPointLight()
		{
			m_writer.ImplementFunction< Light >( cuT( "GetPointLight" ), [this]( Int const & p_index )
			{
				LOCALE_ASSIGN( m_writer, Light, l_lightReturn, GetLightColourAndPosition( p_index ) );

				if ( m_writer.HasTexelFetch() && m_writer.GetOpenGl().HasTbo() )
				{
					BUILTIN( m_writer, SamplerBuffer, c3d_sLights );
					LOCALE_ASSIGN( m_writer, Int, l_offset, p_index * Int( 10 ) + Int( 3 ) );
					l_lightReturn.m_v3Attenuation() = texelFetch( c3d_sLights, l_offset++ ).XYZ;
				}
				else
				{
					BUILTIN( m_writer, Sampler1D, c3d_sLights );
					LOCALE_ASSIGN( m_writer, Float, l_factor, p_index * Float( 0.01f ) );
					LOCALE_ASSIGN( m_writer, Float, l_mult, Float( 0.001f ) );
					LOCALE_ASSIGN( m_writer, Float, l_offset, l_mult * Float( 3 ) );
					LOCALE_ASSIGN( m_writer, Float, l_decal, Float( 0.0005f ) );
					l_lightReturn.m_v3Attenuation() = texture1D( c3d_sLights, l_factor + l_offset + l_decal ).XYZ;
				}

				m_writer.Return( l_lightReturn );

			}, Int( &m_writer, cuT( "p_index" ) ) );
		}

		void LightingModel::Declare_GetSpotLight()
		{
			m_writer.ImplementFunction< Light >( cuT( "GetSpotLight" ), [this]( Int const & p_index )
			{
				LOCALE_ASSIGN( m_writer, Light, l_lightReturn, GetLightColourAndPosition( p_index ) );

				if ( m_writer.HasTexelFetch() && m_writer.GetOpenGl().HasTbo() )
				{
					BUILTIN( m_writer, SamplerBuffer, c3d_sLights );
					LOCALE_ASSIGN( m_writer, Int, l_offset, p_index * Int( 10 ) + Int( 3 ) );
					l_lightReturn.m_v3Attenuation() = texelFetch( c3d_sLights, l_offset++ ).XYZ;
					l_lightReturn.m_v3Direction() = texelFetch( c3d_sLights, l_offset++ ).XYZ;
					LOCALE_ASSIGN( m_writer, Vec2, l_v2Spot, texelFetch( c3d_sLights, l_offset++ ).XY );
					l_lightReturn.m_fExponent() = l_v2Spot.X;
					l_lightReturn.m_fCutOff() = l_v2Spot.Y;
				}
				else
				{
					BUILTIN( m_writer, Sampler1D, c3d_sLights );
					LOCALE_ASSIGN( m_writer, Float, l_factor, p_index * Float( 0.01f ) );
					LOCALE_ASSIGN( m_writer, Float, l_mult, Float( 0.001f ) );
					LOCALE_ASSIGN( m_writer, Float, l_offset, l_mult * Float( 3 ) );
					LOCALE_ASSIGN( m_writer, Float, l_decal, Float( 0.0005f ) );
					l_lightReturn.m_v3Attenuation() = texture1D( c3d_sLights, l_factor + l_offset + l_decal ).XYZ;
					l_offset += l_mult;
					l_lightReturn.m_v3Direction() = texture1D( c3d_sLights, l_factor + l_offset + l_decal ).XYZ;
					l_offset += l_mult;
					LOCALE_ASSIGN( m_writer, Vec2, l_v2Spot, texture1D( c3d_sLights, l_factor + l_offset + l_decal ).XY );
					l_lightReturn.m_fExponent() = l_v2Spot.X;
					l_lightReturn.m_fCutOff() = l_v2Spot.Y;
				}

				m_writer.Return( l_lightReturn );

			}, Int( &m_writer, cuT( "p_index" ) ) );
		}

		Light LightingModel::GetLightColourAndPosition( Type const & p_value )
		{
			return WriteFunctionCall< Light >( &m_writer, cuT( "GetLightColourAndPosition" ), p_value );
		}

		Light LightingModel::GetDirectionalLight( Type const & p_value )
		{
			return WriteFunctionCall< Light >( &m_writer, cuT( "GetDirectionalLight" ), p_value );
		}

		Light LightingModel::GetPointLight( Type const & p_value )
		{
			return WriteFunctionCall< Light >( &m_writer, cuT( "GetPointLight" ), p_value );
		}

		Light LightingModel::GetSpotLight( Type const & p_value )
		{
			return WriteFunctionCall< Light >( &m_writer, cuT( "GetSpotLight" ), p_value );
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
				LOCALE( m_writer, Vec3, l_ambient );
				LOCALE( m_writer, Vec3, l_diffuse );
				LOCALE( m_writer, Vec3, l_specular );
				OutputComponents l_output = { l_ambient, l_diffuse, l_specular };
				DoComputeLight( p_light, p_worldEye, normalize( p_light.m_v3Position().XYZ ), p_shininess, p_fragmentIn, l_output );
				p_output.m_v3Ambient += l_ambient;
				p_output.m_v3Diffuse += l_diffuse;
				p_output.m_v3Specular += l_specular;

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
				LOCALE( m_writer, Vec3, l_ambient );
				LOCALE( m_writer, Vec3, l_diffuse );
				LOCALE( m_writer, Vec3, l_specular );
				OutputComponents l_output = { l_ambient, l_diffuse, l_specular };
				LOCALE_ASSIGN( m_writer, Vec3, l_lightDirection, p_fragmentIn.m_v3Vertex - p_light.m_v3Position().XYZ );
				LOCALE_ASSIGN( m_writer, Float, l_distance, length( l_lightDirection ) );
				DoComputeLight( p_light, p_worldEye, normalize( -l_lightDirection ), p_shininess, p_fragmentIn, l_output );

				LOCALE_ASSIGN( m_writer, Float, l_attenuation, p_light.m_v3Attenuation().X + p_light.m_v3Attenuation().Y * l_distance + p_light.m_v3Attenuation().Z * l_distance * l_distance );
				p_output.m_v3Ambient += l_ambient / l_attenuation;
				p_output.m_v3Diffuse += l_diffuse / l_attenuation;
				p_output.m_v3Specular += l_specular / l_attenuation;

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
				LOCALE( m_writer, Vec3, l_ambient );
				LOCALE( m_writer, Vec3, l_diffuse );
				LOCALE( m_writer, Vec3, l_specular );
				OutputComponents l_output = { l_ambient, l_diffuse, l_specular };
				LOCALE_ASSIGN( m_writer, Vec3, l_lightToPixel, p_fragmentIn.m_v3Vertex - p_light.m_v3Position().XYZ );
				LOCALE_ASSIGN( m_writer, Float, l_spotFactor, dot( l_lightToPixel, p_light.m_v3Direction() ) );

				IF( m_writer, l_spotFactor > p_light.m_fCutOff() )
				{
					LOCALE_ASSIGN( m_writer, Vec3, l_lightDirection, p_fragmentIn.m_v3Vertex - p_light.m_v3Position().XYZ );
					LOCALE_ASSIGN( m_writer, Float, l_distance, length( l_lightDirection ) );
					ComputePointLight( p_light, p_worldEye, p_shininess, p_fragmentIn, l_output );

					l_spotFactor = m_writer.Paren( Float( 1 ) - m_writer.Paren( Float( 1 ) - l_spotFactor ) * Float( 1 ) / m_writer.Paren( Float( 1 ) - p_light.m_fCutOff() ) );
					p_output.m_v3Ambient += l_ambient * l_spotFactor;
					p_output.m_v3Diffuse += l_diffuse * l_spotFactor;
					p_output.m_v3Specular += l_specular * l_spotFactor;
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
				p_output.m_v3Ambient = p_light.m_v3Colour() * p_light.m_v3Intensity().X;

				LOCALE_ASSIGN( m_writer, Float, l_diffuseFactor, dot( p_fragmentIn.m_v3Normal, p_direction ) );

				IF( m_writer, l_diffuseFactor > Float( 0 ) )
				{
					p_output.m_v3Diffuse = p_light.m_v3Colour() * p_light.m_v3Intensity().Y * l_diffuseFactor;

					LOCALE_ASSIGN( m_writer, Vec3, l_vertexToEye, normalize( p_worldEye - p_fragmentIn.m_v3Vertex ) );
					LOCALE_ASSIGN( m_writer, Vec3, l_lightReflect, normalize( reflect( p_direction, p_fragmentIn.m_v3Normal ) ) );
					LOCALE_ASSIGN( m_writer, Float, l_specularFactor, -dot( l_vertexToEye, l_lightReflect ) );

					IF( m_writer, l_specularFactor > Float( 0 ) )
					{
						l_specularFactor = pow( l_specularFactor, p_shininess );
						p_output.m_v3Specular = p_light.m_v3Colour() * p_light.m_v3Intensity().Z * l_specularFactor;
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
