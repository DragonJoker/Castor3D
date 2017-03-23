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
		, m_shadowModel{ p_writer }
	{
	}

	void LightingModel::DeclareModel()
	{
		if ( m_shadows != ShadowType::eNone )
		{
			m_shadowModel.Declare( m_shadows );
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

	void LightingModel::DeclareDirectionalModel()
	{
		if ( m_shadows != ShadowType::eNone )
		{
			m_shadowModel.DeclareDirectional( m_shadows );
		}

		Declare_Light();
		Declare_DirectionalLight();
		DoDeclareModel();
		Declare_ComputeDirectionalLight();
	}

	void LightingModel::DeclarePointModel()
	{
		if ( m_shadows != ShadowType::eNone )
		{
			m_shadowModel.DeclarePoint( m_shadows );
		}

		Declare_Light();
		Declare_PointLight();
		DoDeclareModel();
		Declare_ComputeOnePointLight();
	}

	void LightingModel::DeclareSpotModel()
	{
		if ( m_shadows != ShadowType::eNone )
		{
			m_shadowModel.DeclareSpot( m_shadows );
		}

		Declare_Light();
		Declare_SpotLight();
		DoDeclareModel();
		Declare_ComputeOneSpotLight();
	}

	DirectionalLight LightingModel::GetDirectionalLight( Type const & p_value )
	{
		return WriteFunctionCall< DirectionalLight >( &m_writer
			, cuT( "GetDirectionalLight" )
			, p_value );
	}

	PointLight LightingModel::GetPointLight( Type const & p_value )
	{
		return WriteFunctionCall< PointLight >( &m_writer
			, cuT( "GetPointLight" )
			, p_value );
	}

	SpotLight LightingModel::GetSpotLight( Type const & p_value )
	{
		return WriteFunctionCall< SpotLight >( &m_writer
			, cuT( "GetSpotLight" )
			, p_value );
	}

	void LightingModel::ComputeCombinedLighting( Vec3 const & p_worldEye
		, Float const & p_shininess
		, Int const & p_receivesShadows
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
				, p_receivesShadows
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
				, p_receivesShadows
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
				, p_receivesShadows
				, p_fragmentIn
				, p_output );
		}
		ROF;
	}
	void LightingModel::ComputeDirectionalLight( DirectionalLight const & p_light
		, Vec3 const & p_worldEye
		, Float const & p_shininess
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		m_writer << m_computeDirectional( DirectionalLight{ p_light }
			, p_worldEye
			, p_shininess
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn }
			, p_output );
		m_writer << Endi();
	}

	void LightingModel::ComputePointLight( PointLight const & p_light
		, Vec3 const & p_worldEye
		, Float const & p_shininess
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		m_writer << m_computePoint( PointLight{ p_light }
			, p_worldEye
			, p_shininess
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn }
			, p_output );
		m_writer << Endi();
	}

	void LightingModel::ComputeSpotLight( SpotLight const & p_light
		, Vec3 const & p_worldEye
		, Float const & p_shininess
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		m_writer << m_computeSpot( SpotLight{ p_light }
			, p_worldEye
			, p_shininess
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn }
			, p_output );
		m_writer << Endi();
	}

	void LightingModel::ComputeOnePointLight( PointLight const & p_light
		, Vec3 const & p_worldEye
		, Float const & p_shininess
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		m_writer << m_computeOnePoint( PointLight{ p_light }
			, p_worldEye
			, p_shininess
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn }
			, p_output );
		m_writer << Endi();
	}

	void LightingModel::ComputeOneSpotLight( SpotLight const & p_light
		, Vec3 const & p_worldEye
		, Float const & p_shininess
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		m_writer << m_computeOneSpot( SpotLight{ p_light }
			, p_worldEye
			, p_shininess
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn }
			, p_output );
		m_writer << Endi();
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
		l_lightDecl.GetMember< Int >( cuT( "m_iIndex" ) );
		l_lightDecl.End();
	}

	void LightingModel::Declare_SpotLight()
	{
		Struct l_lightDecl = m_writer.GetStruct( cuT( "SpotLight" ) );
		l_lightDecl.GetMember< Light >( cuT( "m_lightBase" ) );
		l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Position" ) );
		l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Attenuation" ) );
		l_lightDecl.GetMember< Int >( cuT( "m_iIndex" ) );
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
					auto l_v4MtxCol1 = m_writer.GetLocale( cuT( "l_v4MtxCol1" ), texelFetch( c3d_sLights, l_offset++ ) );
					auto l_v4MtxCol2 = m_writer.GetLocale( cuT( "l_v4MtxCol2" ), texelFetch( c3d_sLights, l_offset++ ) );
					auto l_v4MtxCol3 = m_writer.GetLocale( cuT( "l_v4MtxCol3" ), texelFetch( c3d_sLights, l_offset++ ) );
					auto l_v4MtxCol4 = m_writer.GetLocale( cuT( "l_v4MtxCol4" ), texelFetch( c3d_sLights, l_offset++ ) );
					l_lightReturn.m_mtxLightSpace() = mat4( l_v4MtxCol1, l_v4MtxCol2, l_v4MtxCol3, l_v4MtxCol4 );
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					l_lightReturn.m_v3Direction() = texelFetch( c3d_sLights, l_offset++, 0 ).rgb();
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
				auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), l_mult * Float( BaseLightComponentsCount ) );
				l_lightReturn.m_v3Direction() = texture( c3d_sLights, l_factor + l_offset + l_decal ).rgb();
				l_offset += l_mult;
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
					auto l_v4PosIndex = m_writer.GetLocale( cuT( "l_v4PosIndex" ), texelFetch( c3d_sLights, l_offset++ ) );
					l_lightReturn.m_v3Position() = l_v4PosIndex.rgb();
					l_lightReturn.m_iIndex() = m_writer.Cast< Int >( l_v4PosIndex.a() );
					l_lightReturn.m_v3Attenuation() = texelFetch( c3d_sLights, l_offset++ ).rgb();
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					auto l_v4PosIndex = m_writer.GetLocale( cuT( "l_v4PosIndex" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					l_lightReturn.m_v3Position() = l_v4PosIndex.rgb();
					l_lightReturn.m_iIndex() = m_writer.Cast< Int >( l_v4PosIndex.a() );
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
				auto l_v4PosIndex = m_writer.GetLocale( cuT( "l_v4PosIndex" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_lightReturn.m_v3Position() = l_v4PosIndex.rgb();
				l_lightReturn.m_iIndex() = m_writer.Cast< Int >( l_v4PosIndex.a() );
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
			l_lightReturn.m_lightBase() = GetBaseLight( p_index );

			if ( m_writer.HasTexelFetch() )
			{
				if ( m_writer.HasTextureBuffers() )
				{
					auto c3d_sLights = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					auto l_v4PosIndex = m_writer.GetLocale( cuT( "l_v4PosIndex" ), texelFetch( c3d_sLights, l_offset++ ) );
					l_lightReturn.m_v3Position() = l_v4PosIndex.rgb();
					l_lightReturn.m_iIndex() = m_writer.Cast< Int >( l_v4PosIndex.a() );
					l_lightReturn.m_v3Attenuation() = texelFetch( c3d_sLights, l_offset++ ).rgb();
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
					auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					auto l_v4PosIndex = m_writer.GetLocale( cuT( "l_v4PosIndex" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					l_lightReturn.m_v3Position() = l_v4PosIndex.rgb();
					l_lightReturn.m_iIndex() = m_writer.Cast< Int >( l_v4PosIndex.a() );
					l_lightReturn.m_v3Attenuation() = texelFetch( c3d_sLights, l_offset++, 0 ).rgb();
					l_lightReturn.m_v3Direction() = normalize( texelFetch( c3d_sLights, l_offset++, 0 ).rgb() );
					auto l_v2Spot = m_writer.GetLocale( cuT( "l_v2Spot" ), texelFetch( c3d_sLights, l_offset++, 0 ).rg() );
					l_lightReturn.m_fExponent() = l_v2Spot.x();
					l_lightReturn.m_fCutOff() = l_v2Spot.y();
					auto l_v4MtxCol1 = m_writer.GetLocale( cuT( "l_v4MtxCol1" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					auto l_v4MtxCol2 = m_writer.GetLocale( cuT( "l_v4MtxCol2" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					auto l_v4MtxCol3 = m_writer.GetLocale( cuT( "l_v4MtxCol3" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					auto l_v4MtxCol4 = m_writer.GetLocale( cuT( "l_v4MtxCol4" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					l_lightReturn.m_mtxLightSpace() = mat4( l_v4MtxCol1, l_v4MtxCol2, l_v4MtxCol3, l_v4MtxCol4 );
					l_lightReturn.m_iIndex() = m_writer.Cast< Int >( texelFetch( c3d_sLights, l_offset++, 0 ).r() );
				}
			}
			else
			{
				auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
				auto l_factor = m_writer.GetLocale( cuT( "l_factor" ), Float( LightComponentsOffset ) * p_index );
				auto l_decal = m_writer.GetLocale( cuT( "l_decal" ), 0.0005_f );
				auto l_mult = m_writer.GetLocale( cuT( "l_mult" ), 0.001_f );
				auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), l_mult * Float( BaseLightComponentsCount ) );
				auto l_v4PosIndex = m_writer.GetLocale( cuT( "l_v4PosIndex" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_lightReturn.m_v3Position() = l_v4PosIndex.rgb();
				l_lightReturn.m_iIndex() = m_writer.Cast< Int >( l_v4PosIndex.a() );
				l_offset += l_mult;
				l_lightReturn.m_v3Attenuation() = texture( c3d_sLights, l_factor + l_offset + l_decal ).rgb();
				l_offset += l_mult;
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
				l_lightReturn.m_iIndex() = m_writer.Cast< Int >( texture( c3d_sLights, l_offset + l_decal ).r() );
				l_offset += l_mult;
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
		m_computeDirectional = m_writer.ImplementFunction< Void >( cuT( "ComputeDirectionalLight" )
			, [this]( DirectionalLight const & p_light
			, Vec3 const & p_worldEye
			, Float const & p_shininess
			, Int const & p_receivesShadows
			, FragmentInput const & p_fragmentIn
			, OutputComponents & p_output )
			{
				OutputComponents l_output
				{
					m_writer.GetLocale( cuT( "l_ambient" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.GetLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.GetLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto l_lightDirection = m_writer.GetLocale( cuT( "l_lightDirection" ), normalize( p_light.m_v3Direction().xyz() ) );
				auto l_shadowFactor = m_writer.GetLocale( cuT( "l_shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					l_shadowFactor = 1.0_f - min( p_receivesShadows
						, m_shadowModel.ComputeDirectionalShadow( p_light.m_mtxLightSpace()
							, p_fragmentIn.m_v3Vertex
							, l_lightDirection
							, p_fragmentIn.m_v3Normal ) );
				}

				DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, l_lightDirection
					, p_shininess
					, l_shadowFactor
					, p_fragmentIn
					, l_output );
				p_output.m_v3Ambient += l_output.m_v3Ambient;
				p_output.m_v3Diffuse += l_output.m_v3Diffuse;
				p_output.m_v3Specular += l_output.m_v3Specular;
			}
			, DirectionalLight( &m_writer, cuT( "p_light" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) )
			, InParam< Float >( &m_writer, cuT( "p_shininess" ) )
			, InParam< Int >( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, l_output );
	}

	void PhongLightingModel::Declare_ComputePointLight()
	{
		OutputComponents l_output{ m_writer };
		m_computePoint = m_writer.ImplementFunction< Void >( cuT( "ComputePointLight" )
			, [this]( PointLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents l_output
				{
					m_writer.GetLocale( cuT( "l_ambient" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.GetLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.GetLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto l_lightToVertex = m_writer.GetLocale( cuT( "l_lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_v3Position().xyz() );
				auto l_distance = m_writer.GetLocale( cuT( "l_distance" ), length( l_lightToVertex ) );
				auto l_lightDirection = m_writer.GetLocale( cuT( "l_lightDirection" ), normalize( l_lightToVertex ) );
				auto l_shadowFactor = m_writer.GetLocale( cuT( "l_shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					IF( m_writer, p_light.m_iIndex() >= 0_i )
					{
						l_shadowFactor = 1.0_f - min( p_receivesShadows
							, m_shadowModel.ComputePointShadow( p_fragmentIn.m_v3Vertex
								, p_light.m_v3Position().xyz()
								, p_fragmentIn.m_v3Normal
								, p_light.m_iIndex() ) );
					}
					FI;
				}

				DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, l_lightDirection
					, p_shininess
					, l_shadowFactor
					, p_fragmentIn
					, l_output );
				auto l_attenuation = m_writer.GetLocale( cuT( "l_attenuation" )
					, p_light.m_v3Attenuation().x()
					+ p_light.m_v3Attenuation().y() * l_distance
					+ p_light.m_v3Attenuation().z() * l_distance * l_distance );
				p_output.m_v3Ambient += l_output.m_v3Ambient / l_attenuation;
				p_output.m_v3Diffuse += l_output.m_v3Diffuse / l_attenuation;
				p_output.m_v3Specular += l_output.m_v3Specular / l_attenuation;
			}
			, PointLight( &m_writer, cuT( "p_light" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) )
			, InParam< Float >( &m_writer, cuT( "p_shininess" ) )
			, InParam< Int >( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, l_output );
	}

	void PhongLightingModel::Declare_ComputeSpotLight()
	{
		OutputComponents l_output{ m_writer };
		m_computeSpot = m_writer.ImplementFunction< Void >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents l_output
				{
					m_writer.GetLocale( cuT( "l_ambient" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.GetLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.GetLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto l_lightToVertex = m_writer.GetLocale( cuT( "l_lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_v3Position().xyz() );
				auto l_distance = m_writer.GetLocale( cuT( "l_distance" ), length( l_lightToVertex ) );
				auto l_lightDirection = m_writer.GetLocale( cuT( "l_lightDirection" ), normalize( l_lightToVertex ) );
				auto l_spotFactor = m_writer.GetLocale( cuT( "l_spotFactor" ), dot( l_lightDirection, p_light.m_v3Direction() ) );

				IF( m_writer, l_spotFactor > p_light.m_fCutOff() )
				{
					auto l_shadowFactor = m_writer.GetLocale( cuT( "l_shadowFactor" ), Float( 1 ) );

					if ( m_shadows != ShadowType::eNone )
					{
						IF( m_writer, p_light.m_iIndex() >= 0_i )
						{
							l_shadowFactor = 1.0_f - min( p_receivesShadows
								, m_shadowModel.ComputeSpotShadow( p_light.m_mtxLightSpace()
									, p_fragmentIn.m_v3Vertex
									, l_lightToVertex
									, p_fragmentIn.m_v3Normal
									, p_light.m_iIndex() ) );
						}
						FI;
					}

					DoComputeLight( p_light.m_lightBase()
						, p_worldEye
						, l_lightDirection
						, p_shininess
						, l_shadowFactor
						, p_fragmentIn
						, l_output );
					auto l_attenuation = m_writer.GetLocale( cuT( "l_attenuation" )
						, p_light.m_v3Attenuation().x()
						+ p_light.m_v3Attenuation().y() * l_distance
						+ p_light.m_v3Attenuation().z() * l_distance * l_distance );
					l_spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - l_spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - p_light.m_fCutOff() ) );
					p_output.m_v3Ambient += l_spotFactor * l_output.m_v3Ambient / l_attenuation;
					p_output.m_v3Diffuse += l_spotFactor * l_output.m_v3Diffuse / l_attenuation;
					p_output.m_v3Specular += l_spotFactor * l_output.m_v3Specular / l_attenuation;
				}
				FI;
			}
			, SpotLight( &m_writer, cuT( "p_light" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) )
			, InParam< Float >( &m_writer, cuT( "p_shininess" ) )
			, InParam< Int >( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, l_output );
	}

	void PhongLightingModel::Declare_ComputeOnePointLight()
	{
		OutputComponents l_output{ m_writer };
		m_computeOnePoint = m_writer.ImplementFunction< Void >( cuT( "ComputePointLight" )
			, [this]( PointLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents l_output
				{
					m_writer.GetLocale( cuT( "l_ambient" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.GetLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.GetLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto l_lightToVertex = m_writer.GetLocale( cuT( "l_lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_v3Position().xyz() );
				auto l_distance = m_writer.GetLocale( cuT( "l_distance" ), length( l_lightToVertex ) );
				auto l_lightDirection = m_writer.GetLocale( cuT( "l_lightDirection" ), normalize( l_lightToVertex ) );
				auto l_shadowFactor = m_writer.GetLocale( cuT( "l_shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					l_shadowFactor = 1.0_f - min( p_receivesShadows
						, m_shadowModel.ComputePointShadow( p_fragmentIn.m_v3Vertex
							, p_light.m_v3Position().xyz()
							, p_fragmentIn.m_v3Normal ) );
				}

				DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, l_lightDirection
					, p_shininess
					, l_shadowFactor
					, p_fragmentIn
					, l_output );
				auto l_attenuation = m_writer.GetLocale( cuT( "l_attenuation" )
					, p_light.m_v3Attenuation().x()
					+ p_light.m_v3Attenuation().y() * l_distance
					+ p_light.m_v3Attenuation().z() * l_distance * l_distance );
				p_output.m_v3Ambient += l_output.m_v3Ambient / l_attenuation;
				p_output.m_v3Diffuse += l_output.m_v3Diffuse / l_attenuation;
				p_output.m_v3Specular += l_output.m_v3Specular / l_attenuation;
			}
			, PointLight( &m_writer, cuT( "p_light" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) )
			, InParam< Float >( &m_writer, cuT( "p_shininess" ) )
			, InParam< Int >( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, l_output );
	}

	void PhongLightingModel::Declare_ComputeOneSpotLight()
	{
		OutputComponents l_output{ m_writer };
		m_computeOneSpot = m_writer.ImplementFunction< Void >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents l_output
				{
					m_writer.GetLocale( cuT( "l_ambient" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.GetLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.GetLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto l_lightToVertex = m_writer.GetLocale( cuT( "l_lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_v3Position().xyz() );
				auto l_distance = m_writer.GetLocale( cuT( "l_distance" ), length( l_lightToVertex ) );
				auto l_lightDirection = m_writer.GetLocale( cuT( "l_lightDirection" ), normalize( l_lightToVertex ) );
				auto l_spotFactor = m_writer.GetLocale( cuT( "l_spotFactor" ), dot( l_lightDirection, p_light.m_v3Direction() ) );

				IF( m_writer, l_spotFactor > p_light.m_fCutOff() )
				{
					auto l_shadowFactor = m_writer.GetLocale( cuT( "l_shadowFactor" ), Float( 1 ) );

					if ( m_shadows != ShadowType::eNone )
					{
						l_shadowFactor = 1.0_f - min( p_receivesShadows
							, m_shadowModel.ComputeSpotShadow( p_light.m_mtxLightSpace()
								, p_fragmentIn.m_v3Vertex
								, l_lightToVertex
								, p_fragmentIn.m_v3Normal ) );
					}

					DoComputeLight( p_light.m_lightBase()
						, p_worldEye
						, l_lightDirection
						, p_shininess
						, l_shadowFactor
						, p_fragmentIn
						, l_output );
					auto l_attenuation = m_writer.GetLocale( cuT( "l_attenuation" )
						, p_light.m_v3Attenuation().x()
						+ p_light.m_v3Attenuation().y() * l_distance
						+ p_light.m_v3Attenuation().z() * l_distance * l_distance );
					l_spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - l_spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - p_light.m_fCutOff() ) );
					p_output.m_v3Ambient += l_spotFactor * l_output.m_v3Ambient / l_attenuation;
					p_output.m_v3Diffuse += l_spotFactor * l_output.m_v3Diffuse / l_attenuation;
					p_output.m_v3Specular += l_spotFactor * l_output.m_v3Specular / l_attenuation;
				}
				FI;
			}
			, SpotLight( &m_writer, cuT( "p_light" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) )
			, InParam< Float >( &m_writer, cuT( "p_shininess" ) )
			, InParam< Int >( &m_writer, cuT( "p_receivesShadows" ) )
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
			, Float const & p_shadowFactor
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
				p_output.m_v3Diffuse = p_shadowFactor * p_light.m_v3Colour() * p_light.m_v3Intensity().y() * l_diffuseFactor;
				p_output.m_v3Specular = p_shadowFactor * p_light.m_v3Colour() * p_light.m_v3Intensity().z() * pow( l_specularFactor, max( p_shininess, 0.1_f ) );
			}
			FI;
		};
		m_writer.ImplementFunction< Void >( cuT( "DoComputeLight" )
			, l_compute
			, InParam< Light >( &m_writer, cuT( "p_light" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_worldEye" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_direction" ) )
			, InParam< Float >( &m_writer, cuT( "p_shininess" ) )
			, InParam< Float >( &m_writer, cuT( "p_shadowFactor" ) )
			, FragmentInput{ m_writer }
			, l_output );
	}

	void PhongLightingModel::DoComputeLight( Light const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_direction
		, Float const & p_shininess
		, Float const & p_shadowFactor
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		m_writer << WriteFunctionCall< Vec3 >( &m_writer, cuT( "DoComputeLight" )
			, p_light
			, p_worldEye
			, p_direction
			, p_shininess
			, p_shadowFactor
			, p_fragmentIn
			, p_output ) << Endi();
	}

	//***********************************************************************************************
}
