#include "GlslLighting.hpp"

#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	//***********************************************************************************************

	String ParamToString( String & p_sep, FragmentInput const & p_value )
	{
		StringStream l_return;
		l_return << ParamToString( p_sep, p_value.m_v3Vertex );
		l_return << ParamToString( p_sep, p_value.m_v3Normal );
		return l_return.str();
	}

	String ParamToString( String & p_sep, OutputComponents const & p_value )
	{
		StringStream l_return;
		l_return << ParamToString( p_sep, p_value.m_v3Ambient );
		l_return << ParamToString( p_sep, p_value.m_v3Diffuse );
		l_return << ParamToString( p_sep, p_value.m_v3Specular );
		return l_return.str();
	}

	String ToString( FragmentInput const & p_value )
	{
		StringStream l_return;
		l_return << ToString( p_value.m_v3Vertex ) << ", ";
		l_return << ToString( p_value.m_v3Normal );
		return l_return.str();
	}

	String ToString( OutputComponents const & p_value )
	{
		StringStream l_return;
		l_return << ToString( p_value.m_v3Ambient ) << ", ";
		l_return << ToString( p_value.m_v3Diffuse ) << ", ";
		l_return << ToString( p_value.m_v3Specular );
		return l_return.str();
	}

	//***********************************************************************************************

	FragmentInput::FragmentInput( GlslWriter & p_writer )
		: m_v3Vertex{ &p_writer, cuT( "p_v3Vertex" ) }
		, m_v3Normal{ &p_writer, cuT( "p_v3Normal" ) }
	{
	}

	FragmentInput::FragmentInput( InParam< Vec3 > const & p_v3Vertex
								  , InParam< Vec3 > const & p_v3Normal )
		: m_v3Vertex{ p_v3Vertex }
		, m_v3Normal{ p_v3Normal }
	{
	}

	//***********************************************************************************************

	OutputComponents::OutputComponents( GlslWriter & p_writer )
		: m_v3Ambient{ &p_writer, cuT( "p_v3Ambient" ) }
		, m_v3Diffuse{ &p_writer, cuT( "p_v3Diffuse" ) }
		, m_v3Specular{ &p_writer, cuT( "p_v3Specular" ) }
	{
	}

	OutputComponents::OutputComponents( InOutParam< Vec3 > const & p_v3Ambient
										, InOutParam< Vec3 > const & p_v3Diffuse
										, InOutParam< Vec3 > const & p_v3Specular )
		: m_v3Ambient{ p_v3Ambient }
		, m_v3Diffuse{ p_v3Diffuse }
		, m_v3Specular{ p_v3Specular }
	{
	}

	//***********************************************************************************************

	LightingModel::LightingModel( ShadowType p_shadows, GlslWriter & p_writer )
		: m_shadows{ p_shadows }
		, m_writer{ p_writer }
	{
	}

	void LightingModel::DeclareModel()
	{
		if ( m_shadows != ShadowType::eNone )
		{
			Shadow l_shadow{ m_writer };
			l_shadow.Declare( m_shadows );
		}

		Declare_Light();
		Declare_DirectionalLight();
		Declare_PointLight();
		Declare_SpotLight();
		Declare_GetBaseLight();
		Declare_GetDirectionalLight();
		Declare_GetPointLight();
		Declare_GetSpotLight();
		DoDeclareModel();
		Declare_ComputeDirectionalLight();
		Declare_ComputePointLight();
		Declare_ComputeSpotLight();
	}

	void LightingModel::ComputeCombinedLighting( Vec3 const & p_worldEye
												 , Float const & p_shininess
												 , FragmentInput const & p_fragmentIn
												 , OutputComponents & p_output )
	{
		auto c3d_iLightsCount = m_writer.GetBuiltin< Vec3 >( cuT( "c3d_iLightsCount" ) );
		auto l_begin = m_writer.GetLocale( cuT( "l_begin" ), 0_i );
		auto l_end = m_writer.GetLocale( cuT( "l_end" ), m_writer.Cast< Int >( c3d_iLightsCount.x() ) );

		FOR( m_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
		{
			ComputeDirectionalLight( GetDirectionalLight( i )
									 , p_worldEye
									 , p_shininess
									 , p_fragmentIn
									 , p_output );
		}
		ROF;

		l_begin = l_end;
		l_end += m_writer.Cast< Int >( c3d_iLightsCount.y() );

		FOR( m_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
		{
			ComputePointLight( GetPointLight( i )
							   , p_worldEye
							   , p_shininess
							   , p_fragmentIn
							   , p_output );
		}
		ROF;

		l_begin = l_end;
		l_end += m_writer.Cast< Int >( c3d_iLightsCount.z() );

		FOR( m_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
		{
			ComputeSpotLight( GetSpotLight( i )
							  , p_worldEye
							  , p_shininess
							  , p_fragmentIn
							  , p_output );
		}
		ROF;
	}

	DirectionalLight LightingModel::GetDirectionalLight( Type const & p_value )
	{
		return WriteFunctionCall< DirectionalLight >( &m_writer, cuT( "GetDirectionalLight" ), p_value );
	}

	PointLight LightingModel::GetPointLight( Type const & p_value )
	{
		return WriteFunctionCall< PointLight >( &m_writer, cuT( "GetPointLight" ), p_value );
	}

	SpotLight LightingModel::GetSpotLight( Type const & p_value )
	{
		return WriteFunctionCall< SpotLight >( &m_writer, cuT( "GetSpotLight" ), p_value );
	}

	void LightingModel::ComputeDirectionalLight( DirectionalLight const & p_light
												 , Vec3 const & p_worldEye
												 , Float const & p_shininess
												 , FragmentInput const & p_fragmentIn
												 , OutputComponents & p_output )
	{
		m_writer << WriteFunctionCall< Void >( &m_writer, cuT( "ComputeDirectionalLight" ), p_light, p_worldEye, p_shininess, p_fragmentIn, p_output ) << Endi();
	}

	void LightingModel::ComputePointLight( PointLight const & p_light
										   , Vec3 const & p_worldEye
										   , Float const & p_shininess
										   , FragmentInput const & p_fragmentIn
										   , OutputComponents & p_output )
	{
		m_writer << WriteFunctionCall< Void >( &m_writer, cuT( "ComputePointLight" ), p_light, p_worldEye, p_shininess, p_fragmentIn, p_output ) << Endi();
	}

	void LightingModel::ComputeSpotLight( SpotLight const & p_light
										  , Vec3 const & p_worldEye
										  , Float const & p_shininess
										  , FragmentInput const & p_fragmentIn
										  , OutputComponents & p_output )
	{
		m_writer << WriteFunctionCall< Void >( &m_writer, cuT( "ComputeSpotLight" ), p_light, p_worldEye, p_shininess, p_fragmentIn, p_output ) << Endi();
	}

	void LightingModel::Declare_Light()
	{
		Struct l_lightDecl = m_writer.GetStruct( cuT( "Light" ) );
		l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Colour" ) );
		l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Intensity" ) );
		l_lightDecl.End();
	}

	void LightingModel::Declare_DirectionalLight()
	{
		Struct l_lightDecl = m_writer.GetStruct( cuT( "DirectionalLight" ) );
		l_lightDecl.GetMember< Light >( cuT( "m_lightBase" ) );
		l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Direction" ) );
		l_lightDecl.GetMember< Mat4 >( cuT( "m_mtxLightSpace" ) );
		l_lightDecl.End();
	}

	void LightingModel::Declare_PointLight()
	{
		Struct l_lightDecl = m_writer.GetStruct( cuT( "PointLight" ) );
		l_lightDecl.GetMember< Light >( cuT( "m_lightBase" ) );
		l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Position" ) );
		l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Attenuation" ) );
		l_lightDecl.End();
	}

	void LightingModel::Declare_SpotLight()
	{
		Struct l_lightDecl = m_writer.GetStruct( cuT( "SpotLight" ) );
		l_lightDecl.GetMember< PointLight >( cuT( "m_lightBase" ) );
		l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Direction" ) );
		l_lightDecl.GetMember< Float >( cuT( "m_fExponent" ) );
		l_lightDecl.GetMember< Float >( cuT( "m_fCutOff" ) );
		l_lightDecl.GetMember< Mat4 >( cuT( "m_mtxLightSpace" ) );
		l_lightDecl.End();
	}

	void LightingModel::Declare_GetBaseLight()
	{
		auto l_get = [this]( Int const & p_index )
		{
			auto l_lightReturn = m_writer.GetLocale< Light >( cuT( "l_lightReturn" ) );

			if ( m_writer.HasTexelFetch() )
			{
				if ( m_writer.HasTextureBuffers() )
				{
					auto c3d_sLights = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) );
					l_lightReturn.m_v3Colour() = texelFetch( c3d_sLights, l_offset++ ).rgb();
					l_lightReturn.m_v3Intensity() = texelFetch( c3d_sLights, l_offset++ ).rgb();
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) );
					l_lightReturn.m_v3Colour() = texelFetch( c3d_sLights, l_offset++, 0 ).rgb();
					l_lightReturn.m_v3Intensity() = texelFetch( c3d_sLights, l_offset++, 0 ).rgb();
				}
			}
			else
			{
				auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
				auto l_factor = m_writer.GetLocale( cuT( "l_factor" ), p_index * Int( LightComponentsOffset ) );
				auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), 0.0_f );
				auto l_decal = m_writer.GetLocale( cuT( "l_decal" ), 0.0005_f );
				auto l_mult = m_writer.GetLocale( cuT( "l_mult" ), 0.001_f );
				l_lightReturn.m_v3Colour() = texture( c3d_sLights, l_factor + l_offset + l_decal ).rgb();
				l_offset += l_mult;
				l_lightReturn.m_v3Intensity() = texture( c3d_sLights, l_factor + l_offset + l_decal ).rgb();
				l_offset += l_mult;
			}

			m_writer.Return( l_lightReturn );
		};
		m_writer.ImplementFunction< Light >( cuT( "GetBaseLight" ), l_get, Int( &m_writer, cuT( "p_index" ) ) );
	}

	void LightingModel::Declare_GetDirectionalLight()
	{
		auto l_get = [this]( Int const & p_index )
		{
			DirectionalLight l_lightReturn = m_writer.GetLocale< DirectionalLight >( cuT( "l_lightReturn" ) );
			l_lightReturn.m_lightBase() = GetBaseLight( p_index );

			if ( m_writer.HasTexelFetch() )
			{
				if ( m_writer.HasTextureBuffers() )
				{
					auto c3d_sLights = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					l_lightReturn.m_v3Direction() = texelFetch( c3d_sLights, l_offset++ ).rgb();
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					l_lightReturn.m_v3Direction() = texelFetch( c3d_sLights, l_offset++, 0 ).rgb();
				}
			}
			else
			{
				auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
				auto l_factor = m_writer.GetLocale( cuT( "l_factor" ), Float( LightComponentsOffset ) * p_index );
				auto l_decal = m_writer.GetLocale( cuT( "l_decal" ), 0.0005_f );
				auto l_mult = m_writer.GetLocale( cuT( "l_mult" ), 0.001_f );
				auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), l_mult * Float( BaseLightComponentsCount ) );
				l_lightReturn.m_v3Direction() = texture( c3d_sLights, l_factor + l_offset + l_decal ).rgb();
				l_offset += l_mult;
			}

			m_writer.Return( l_lightReturn );
		};
		m_writer.ImplementFunction< DirectionalLight >( cuT( "GetDirectionalLight" ), l_get, Int( &m_writer, cuT( "p_index" ) ) );
	}

	void LightingModel::Declare_GetPointLight()
	{
		auto l_get = [this]( Int const & p_index )
		{
			PointLight l_lightReturn = m_writer.GetLocale< PointLight >( cuT( "l_lightReturn" ) );
			l_lightReturn.m_lightBase() = GetBaseLight( p_index );

			if ( m_writer.HasTexelFetch() )
			{
				if ( m_writer.HasTextureBuffers() )
				{
					auto c3d_sLights = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					l_lightReturn.m_v3Position() = texelFetch( c3d_sLights, l_offset++ ).rgb();
					l_lightReturn.m_v3Attenuation() = texelFetch( c3d_sLights, l_offset++ ).rgb();
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					l_lightReturn.m_v3Position() = texelFetch( c3d_sLights, l_offset++, 0 ).rgb();
					l_lightReturn.m_v3Attenuation() = texelFetch( c3d_sLights, l_offset++, 0 ).rgb();
				}
			}
			else
			{
				auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
				auto l_factor = m_writer.GetLocale( cuT( "l_factor" ), Float( LightComponentsOffset ) * p_index );
				auto l_decal = m_writer.GetLocale( cuT( "l_decal" ), 0.0005_f );
				auto l_mult = m_writer.GetLocale( cuT( "l_mult" ), 0.001_f );
				auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), l_mult * Float( BaseLightComponentsCount ) );
				l_lightReturn.m_v3Position() = texture( c3d_sLights, l_factor + l_offset + l_decal ).rgb();
				l_offset += l_mult;
				l_lightReturn.m_v3Attenuation() = texture( c3d_sLights, l_factor + l_offset + l_decal ).rgb();
				l_offset += l_mult;
			}

			m_writer.Return( l_lightReturn );
		};
		m_writer.ImplementFunction< PointLight >( cuT( "GetPointLight" ), l_get, Int( &m_writer, cuT( "p_index" ) ) );
	}

	void LightingModel::Declare_GetSpotLight()
	{
		auto l_get = [this]( Int const & p_index )
		{
			SpotLight l_lightReturn = m_writer.GetLocale< SpotLight >( cuT( "l_lightReturn" ) );
			l_lightReturn.m_lightBase() = GetPointLight( p_index );

			if ( m_writer.HasTexelFetch() )
			{
				if ( m_writer.HasTextureBuffers() )
				{
					auto c3d_sLights = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( PointLightComponentsCount ) );
					l_lightReturn.m_v3Direction() = normalize( texelFetch( c3d_sLights, l_offset++ ).rgb() );
					auto l_v2Spot = m_writer.GetLocale( cuT( "l_v2Spot" ), texelFetch( c3d_sLights, l_offset++ ).rg() );
					l_lightReturn.m_fExponent() = l_v2Spot.x();
					l_lightReturn.m_fCutOff() = l_v2Spot.y();
					auto l_v4MtxCol1 = m_writer.GetLocale( cuT( "l_v4MtxCol1" ), texelFetch( c3d_sLights, l_offset++ ) );
					auto l_v4MtxCol2 = m_writer.GetLocale( cuT( "l_v4MtxCol2" ), texelFetch( c3d_sLights, l_offset++ ) );
					auto l_v4MtxCol3 = m_writer.GetLocale( cuT( "l_v4MtxCol3" ), texelFetch( c3d_sLights, l_offset++ ) );
					auto l_v4MtxCol4 = m_writer.GetLocale( cuT( "l_v4MtxCol4" ), texelFetch( c3d_sLights, l_offset++ ) );
					l_lightReturn.m_mtxLightSpace() = mat4( l_v4MtxCol1, l_v4MtxCol2, l_v4MtxCol3, l_v4MtxCol4 );
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( PointLightComponentsCount ) );
					l_lightReturn.m_v3Direction() = normalize( texelFetch( c3d_sLights, l_offset++, 0 ).rgb() );
					auto l_v2Spot = m_writer.GetLocale( cuT( "l_v2Spot" ), texelFetch( c3d_sLights, l_offset++, 0 ).rg() );
					l_lightReturn.m_fExponent() = l_v2Spot.x();
					l_lightReturn.m_fCutOff() = l_v2Spot.y();
					auto l_v4MtxCol1 = m_writer.GetLocale( cuT( "l_v4MtxCol1" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					auto l_v4MtxCol2 = m_writer.GetLocale( cuT( "l_v4MtxCol2" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					auto l_v4MtxCol3 = m_writer.GetLocale( cuT( "l_v4MtxCol3" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					auto l_v4MtxCol4 = m_writer.GetLocale( cuT( "l_v4MtxCol4" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					l_lightReturn.m_mtxLightSpace() = mat4( l_v4MtxCol1, l_v4MtxCol2, l_v4MtxCol3, l_v4MtxCol4 );
				}
			}
			else
			{
				auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
				auto l_factor = m_writer.GetLocale( cuT( "l_factor" ), Float( LightComponentsOffset ) * p_index );
				auto l_decal = m_writer.GetLocale( cuT( "l_decal" ), 0.0005_f );
				auto l_mult = m_writer.GetLocale( cuT( "l_mult" ), 0.001_f );
				auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), l_mult * Float( PointLightComponentsCount ) );
				l_lightReturn.m_v3Direction() = normalize( texture( c3d_sLights, l_factor + l_offset + l_decal ).rgb() );
				l_offset += l_mult;
				auto l_v2Spot = m_writer.GetLocale( cuT( "l_v2Spot" ), texture( c3d_sLights, l_factor + l_offset + l_decal ).rg() );
				l_lightReturn.m_fExponent() = l_v2Spot.x();
				l_lightReturn.m_fCutOff() = l_v2Spot.y();
				auto l_v4MtxCol1 = m_writer.GetLocale( cuT( "l_v4MtxCol1" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_offset += l_mult;
				auto l_v4MtxCol2 = m_writer.GetLocale( cuT( "l_v4MtxCol2" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_offset += l_mult;
				auto l_v4MtxCol3 = m_writer.GetLocale( cuT( "l_v4MtxCol3" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_offset += l_mult;
				auto l_v4MtxCol4 = m_writer.GetLocale( cuT( "l_v4MtxCol4" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_offset += l_mult;
				l_lightReturn.m_mtxLightSpace() = mat4( l_v4MtxCol1, l_v4MtxCol2, l_v4MtxCol3, l_v4MtxCol4 );
			}

			m_writer.Return( l_lightReturn );
		};
		m_writer.ImplementFunction< SpotLight >( cuT( "GetSpotLight" ), l_get, Int( &m_writer, cuT( "p_index" ) ) );
	}

	Light LightingModel::GetBaseLight( Type const & p_value )
	{
		return WriteFunctionCall< Light >( &m_writer, cuT( "GetBaseLight" ), p_value );
	}

	//***********************************************************************************************

	const String PhongLightingModel::Name = cuT( "phong" );

	PhongLightingModel::PhongLightingModel( ShadowType p_shadows, GlslWriter & p_writer )
		: LightingModel{ p_shadows, p_writer }
	{
	}

	std::unique_ptr< LightingModel > PhongLightingModel::Create( ShadowType p_shadows, GlslWriter & p_writer )
	{
		return std::make_unique< PhongLightingModel >( p_shadows, p_writer );
	}

	void PhongLightingModel::DoDeclareModel()
	{
		DoDeclare_ComputeLight();
	}

	void PhongLightingModel::Declare_ComputeDirectionalLight()
	{
		OutputComponents l_output{ m_writer };
		auto l_compute = [this]( DirectionalLight const & p_light
								 , Vec3 const & p_worldEye
								 , Float const & p_shininess
								 , FragmentInput const & p_fragmentIn
								 , OutputComponents & p_output )
		{
			OutputComponents l_output
			{
				m_writer.GetLocale( cuT( "l_ambient" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
				m_writer.GetLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
				m_writer.GetLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
			};
			DoComputeLight( p_light.m_lightBase(), p_worldEye, normalize( -p_light.m_v3Direction().xyz() ), p_shininess, p_fragmentIn, l_output );
			p_output.m_v3Ambient += l_output.m_v3Ambient;
			p_output.m_v3Diffuse += l_output.m_v3Diffuse;
			p_output.m_v3Specular += l_output.m_v3Specular;
		};
		m_writer.ImplementFunction< Void >( cuT( "ComputeDirectionalLight" )
											, l_compute
											, DirectionalLight( &m_writer, cuT( "p_light" ) )
											, InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) )
											, InParam< Float >( &m_writer, cuT( "p_shininess" ) )
											, FragmentInput{ m_writer }
											, l_output );
	}

	void PhongLightingModel::Declare_ComputePointLight()
	{
		OutputComponents l_output{ m_writer };
		auto l_compute = [this]( PointLight const & p_light
								 , Vec3 const & p_worldEye
								 , Float const & p_shininess
								 , FragmentInput const & p_fragmentIn
								 , OutputComponents & p_output )
		{
			OutputComponents l_output
			{
				m_writer.GetLocale( cuT( "l_ambient" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
				m_writer.GetLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
				m_writer.GetLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
			};
			auto l_vertexToLight = m_writer.GetLocale( cuT( "l_vertexToLight" ), p_fragmentIn.m_v3Vertex - p_light.m_v3Position().xyz() );
			auto l_distance = m_writer.GetLocale( cuT( "l_distance" ), length( l_vertexToLight ) );
			auto l_lightDirection = m_writer.GetLocale( cuT( "l_lightDirection" ), normalize( l_vertexToLight ) );
			DoComputeLight( p_light.m_lightBase(), p_worldEye, l_lightDirection, p_shininess, p_fragmentIn, l_output );

			auto l_attenuation = m_writer.GetLocale( cuT( "l_attenuation" ), p_light.m_v3Attenuation().x() + p_light.m_v3Attenuation().y() * l_distance + p_light.m_v3Attenuation().z() * l_distance * l_distance );
			p_output.m_v3Ambient += l_output.m_v3Ambient / l_attenuation;

			if ( m_shadows != ShadowType::eNone )
			{
				Shadow l_shadows{ m_writer };
				auto l_shadow = m_writer.GetLocale( cuT( "l_shadow" ), l_shadows.ComputePointShadow( l_vertexToLight, p_fragmentIn.m_v3Normal, 0_i ) );
				p_output.m_v3Diffuse += l_shadow * l_output.m_v3Diffuse / l_attenuation;
				p_output.m_v3Specular += l_shadow * l_output.m_v3Specular / l_attenuation;
			}
			else
			{
				p_output.m_v3Diffuse += l_output.m_v3Diffuse / l_attenuation;
				p_output.m_v3Specular += l_output.m_v3Specular / l_attenuation;
			}
		};
		m_writer.ImplementFunction< Void >( cuT( "ComputePointLight" )
											, l_compute
											, PointLight( &m_writer, cuT( "p_light" ) )
											, InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) )
											, InParam< Float >( &m_writer, cuT( "p_shininess" ) )
											, FragmentInput{ m_writer }
											, l_output );
	}

	void PhongLightingModel::Declare_ComputeSpotLight()
	{
		OutputComponents l_output{ m_writer };
		auto l_compute = [this]( SpotLight const & p_light
								 , Vec3 const & p_worldEye
								 , Float const & p_shininess
								 , FragmentInput const & p_fragmentIn
								 , OutputComponents & p_output )
		{
			OutputComponents l_output
			{
				m_writer.GetLocale( cuT( "l_ambient" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
				m_writer.GetLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
				m_writer.GetLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
			};
			auto l_lightToVertex = m_writer.GetLocale( cuT( "l_lightToVertex" ), normalize( p_fragmentIn.m_v3Vertex - p_light.m_lightBase().m_v3Position().xyz() ) );
			auto l_spotFactor = m_writer.GetLocale( cuT( "l_spotFactor" ), dot( l_lightToVertex, p_light.m_v3Direction() ) );

			IF( m_writer, l_spotFactor > p_light.m_fCutOff() )
			{
				ComputePointLight( p_light.m_lightBase(), p_worldEye, p_shininess, p_fragmentIn, l_output );

				l_spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - l_spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - p_light.m_fCutOff() ) );
				p_output.m_v3Ambient += l_output.m_v3Ambient * l_spotFactor;

				if ( m_shadows != ShadowType::eNone )
				{
					Shadow l_shadows{ m_writer };
					auto l_shadow = m_writer.GetLocale( cuT( "l_shadow" ), l_shadows.ComputeSpotShadow( p_light.m_mtxLightSpace() * vec4( p_fragmentIn.m_v3Vertex, 1.0 ), l_lightToVertex, p_fragmentIn.m_v3Normal, 0_i ) );
					p_output.m_v3Diffuse += l_shadow * l_output.m_v3Diffuse * l_spotFactor;
					p_output.m_v3Specular += l_shadow * l_output.m_v3Specular * l_spotFactor;
				}
				else
				{
					p_output.m_v3Diffuse += l_output.m_v3Diffuse * l_spotFactor;
					p_output.m_v3Specular += l_output.m_v3Specular * l_spotFactor;
				}
			}
			FI;
		};
		m_writer.ImplementFunction< Void >( cuT( "ComputeSpotLight" )
											, l_compute
											, SpotLight( &m_writer, cuT( "p_light" ) )
											, InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) )
											, InParam< Float >( &m_writer, cuT( "p_shininess" ) )
											, FragmentInput{ m_writer }
											, l_output );
	}

	void PhongLightingModel::DoDeclare_ComputeLight()
	{
		OutputComponents l_output{ m_writer };
		auto l_compute = [this]( Light const & p_light
								 , Vec3 const & p_worldEye
								 , Vec3 const & p_direction
								 , Float const & p_shininess
								 , FragmentInput const & p_fragmentIn
								 , OutputComponents & p_output )
		{
			p_output.m_v3Ambient = p_light.m_v3Colour() * p_light.m_v3Intensity().x();
			auto l_diffuseFactor = m_writer.GetLocale( cuT( "l_diffuseFactor" ), dot( p_fragmentIn.m_v3Normal, -p_direction ) );

			IF( m_writer, l_diffuseFactor > 0.0_f )
			{
				auto l_vertexToEye = m_writer.GetLocale( cuT( "l_vertexToEye" ), normalize( p_worldEye - p_fragmentIn.m_v3Vertex ) );
				auto l_lightReflect = m_writer.GetLocale( cuT( "l_lightReflect" ), normalize( reflect( p_direction, p_fragmentIn.m_v3Normal ) ) );
				auto l_specularFactor = m_writer.GetLocale( cuT( "l_specularFactor" ), max( dot( l_vertexToEye, l_lightReflect ), 0.0 ) );
				p_output.m_v3Diffuse = p_light.m_v3Colour() * p_light.m_v3Intensity().y() * l_diffuseFactor;
				p_output.m_v3Specular = p_light.m_v3Colour() * p_light.m_v3Intensity().z() * pow( l_specularFactor, p_shininess );
			}
			FI;
		};
		m_writer.ImplementFunction< Void >( cuT( "DoComputeLight" )
											, l_compute
											, InParam< Light >( &m_writer, cuT( "p_light" ) )
											, InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) )
											, InParam< Vec3 >( &m_writer, cuT( "p_direction" ) )
											, InParam< Float >( &m_writer, cuT( "p_shininess" ) )
											, FragmentInput{ m_writer }
											, l_output );
	}

	void PhongLightingModel::DoComputeLight( Light const & p_light
											 , Vec3 const & p_worldEye
											 , Vec3 const & p_direction
											 , Float const & p_shininess
											 , FragmentInput const & p_fragmentIn
											 , OutputComponents & p_output )
	{
		m_writer << WriteFunctionCall< Vec3 >( &m_writer, cuT( "DoComputeLight" )
											   , p_light
											   , p_worldEye
											   , p_direction
											   , p_shininess
											   , p_fragmentIn
											   , p_output ) << Endi();
	}

	//***********************************************************************************************
}
