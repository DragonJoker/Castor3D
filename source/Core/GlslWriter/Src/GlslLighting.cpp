#include "GlslLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	//***********************************************************************************************

	String ParamToString( String & p_sep, FragmentInput const & p_value )
	{
		StringStream l_result;
		l_result << ParamToString( p_sep, p_value.m_v3Vertex );
		l_result << ParamToString( p_sep, p_value.m_v3Normal );
		return l_result.str();
	}

	String ParamToString( String & p_sep, OutputComponents const & p_value )
	{
		StringStream l_result;
		l_result << ParamToString( p_sep, p_value.m_v3Diffuse );
		l_result << ParamToString( p_sep, p_value.m_v3Specular );
		return l_result.str();
	}

	String ToString( FragmentInput const & p_value )
	{
		StringStream l_result;
		l_result << ToString( p_value.m_v3Vertex ) << ", ";
		l_result << ToString( p_value.m_v3Normal );
		return l_result.str();
	}

	String ToString( OutputComponents const & p_value )
	{
		StringStream l_result;
		l_result << ToString( p_value.m_v3Diffuse ) << ", ";
		l_result << ToString( p_value.m_v3Specular );
		return l_result.str();
	}

	//***********************************************************************************************

	FragmentInput::FragmentInput( GlslWriter & p_writer )
		: m_v3Vertex{ &p_writer, cuT( "p_v3Vertex" ) }
		, m_v3Normal{ &p_writer, cuT( "p_v3Normal" ) }
	{
	}

	FragmentInput::FragmentInput( InVec3 const & p_v3Vertex
		, InVec3 const & p_v3Normal )
		: m_v3Vertex{ p_v3Vertex }
		, m_v3Normal{ p_v3Normal }
	{
	}

	//***********************************************************************************************

	OutputComponents::OutputComponents( GlslWriter & p_writer )
		: m_v3Diffuse{ &p_writer, cuT( "p_v3Diffuse" ) }
		, m_v3Specular{ &p_writer, cuT( "p_v3Specular" ) }
	{
	}

	OutputComponents::OutputComponents( InOutVec3 const & p_v3Diffuse
		, InOutVec3 const & p_v3Specular )
		: m_v3Diffuse{ p_v3Diffuse }
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
		Declare_ComputeOneDirectionalLight();
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

	void LightingModel::Declare_Light()
	{
		Struct l_lightDecl = m_writer.GetStruct( cuT( "Light" ) );
		l_lightDecl.DeclMember< Vec3 >( cuT( "m_colour" ) );
		l_lightDecl.DeclMember< Vec2 >( cuT( "m_intensity" ) );
		l_lightDecl.End();
	}

	void LightingModel::Declare_DirectionalLight()
	{
		Struct l_lightDecl = m_writer.GetStruct( cuT( "DirectionalLight" ) );
		l_lightDecl.DeclMember< Light >( cuT( "m_lightBase" ) );
		l_lightDecl.DeclMember< Vec3 >( cuT( "m_direction" ) );
		l_lightDecl.DeclMember< Mat4 >( cuT( "m_transform" ) );
		l_lightDecl.End();
	}

	void LightingModel::Declare_PointLight()
	{
		Struct l_lightDecl = m_writer.GetStruct( cuT( "PointLight" ) );
		l_lightDecl.DeclMember< Light >( cuT( "m_lightBase" ) );
		l_lightDecl.DeclMember< Vec3 >( cuT( "m_position" ) );
		l_lightDecl.DeclMember< Vec3 >( cuT( "m_attenuation" ) );
		l_lightDecl.DeclMember< Int >( cuT( "m_index" ) );
		l_lightDecl.End();
	}

	void LightingModel::Declare_SpotLight()
	{
		Struct l_lightDecl = m_writer.GetStruct( cuT( "SpotLight" ) );
		l_lightDecl.DeclMember< Light >( cuT( "m_lightBase" ) );
		l_lightDecl.DeclMember< Vec3 >( cuT( "m_position" ) );
		l_lightDecl.DeclMember< Vec3 >( cuT( "m_attenuation" ) );
		l_lightDecl.DeclMember< Int >( cuT( "m_index" ) );
		l_lightDecl.DeclMember< Vec3 >( cuT( "m_direction" ) );
		l_lightDecl.DeclMember< Float >( cuT( "m_exponent" ) );
		l_lightDecl.DeclMember< Float >( cuT( "m_cutOff" ) );
		l_lightDecl.DeclMember< Mat4 >( cuT( "m_transform" ) );
		l_lightDecl.End();
	}

	void LightingModel::Declare_GetBaseLight()
	{
		auto l_get = [this]( Int const & p_index )
		{
			auto l_lightReturn = m_writer.DeclLocale< Light >( cuT( "l_lightReturn" ) );

			if ( m_writer.HasTexelFetch() )
			{
				if ( m_writer.HasTextureBuffers() )
				{
					auto c3d_sLights = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.DeclLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) );
					l_lightReturn.m_colour() = texelFetch( c3d_sLights, l_offset++ ).rgb();
					l_lightReturn.m_intensity() = texelFetch( c3d_sLights, l_offset++ ).rg();
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.DeclLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) );
					l_lightReturn.m_colour() = texelFetch( c3d_sLights, l_offset++, 0 ).rgb();
					l_lightReturn.m_intensity() = texelFetch( c3d_sLights, l_offset++, 0 ).rg();
				}
			}
			else
			{
				auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
				auto l_factor = m_writer.DeclLocale( cuT( "l_factor" ), p_index * Int( LightComponentsOffset ) );
				auto l_offset = m_writer.DeclLocale( cuT( "l_offset" ), 0.0_f );
				auto l_decal = m_writer.DeclLocale( cuT( "l_decal" ), 0.0005_f );
				auto l_mult = m_writer.DeclLocale( cuT( "l_mult" ), 0.001_f );
				l_lightReturn.m_colour() = texture( c3d_sLights, l_factor + l_offset + l_decal ).rgb();
				l_offset += l_mult;
				l_lightReturn.m_intensity() = texture( c3d_sLights, l_factor + l_offset + l_decal ).rg();
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
			DirectionalLight l_lightReturn = m_writer.DeclLocale< DirectionalLight >( cuT( "l_lightReturn" ) );
			l_lightReturn.m_lightBase() = GetBaseLight( p_index );

			if ( m_writer.HasTexelFetch() )
			{
				if ( m_writer.HasTextureBuffers() )
				{
					auto c3d_sLights = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.DeclLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					l_lightReturn.m_direction() = texelFetch( c3d_sLights, l_offset++ ).rgb();
					auto l_v4MtxCol1 = m_writer.DeclLocale( cuT( "l_v4MtxCol1" ), texelFetch( c3d_sLights, l_offset++ ) );
					auto l_v4MtxCol2 = m_writer.DeclLocale( cuT( "l_v4MtxCol2" ), texelFetch( c3d_sLights, l_offset++ ) );
					auto l_v4MtxCol3 = m_writer.DeclLocale( cuT( "l_v4MtxCol3" ), texelFetch( c3d_sLights, l_offset++ ) );
					auto l_v4MtxCol4 = m_writer.DeclLocale( cuT( "l_v4MtxCol4" ), texelFetch( c3d_sLights, l_offset++ ) );
					l_lightReturn.m_transform() = mat4( l_v4MtxCol1, l_v4MtxCol2, l_v4MtxCol3, l_v4MtxCol4 );
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.DeclLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					l_lightReturn.m_direction() = texelFetch( c3d_sLights, l_offset++, 0 ).rgb();
					auto l_v4MtxCol1 = m_writer.DeclLocale( cuT( "l_v4MtxCol1" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					auto l_v4MtxCol2 = m_writer.DeclLocale( cuT( "l_v4MtxCol2" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					auto l_v4MtxCol3 = m_writer.DeclLocale( cuT( "l_v4MtxCol3" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					auto l_v4MtxCol4 = m_writer.DeclLocale( cuT( "l_v4MtxCol4" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					l_lightReturn.m_transform() = mat4( l_v4MtxCol1, l_v4MtxCol2, l_v4MtxCol3, l_v4MtxCol4 );
				}
			}
			else
			{
				auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
				auto l_factor = m_writer.DeclLocale( cuT( "l_factor" ), Float( LightComponentsOffset ) * p_index );
				auto l_decal = m_writer.DeclLocale( cuT( "l_decal" ), 0.0005_f );
				auto l_mult = m_writer.DeclLocale( cuT( "l_mult" ), 0.001_f );
				auto l_offset = m_writer.DeclLocale( cuT( "l_offset" ), l_mult * Float( BaseLightComponentsCount ) );
				l_lightReturn.m_direction() = texture( c3d_sLights, l_factor + l_offset + l_decal ).rgb();
				l_offset += l_mult;
				auto l_v4MtxCol1 = m_writer.DeclLocale( cuT( "l_v4MtxCol1" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_offset += l_mult;
				auto l_v4MtxCol2 = m_writer.DeclLocale( cuT( "l_v4MtxCol2" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_offset += l_mult;
				auto l_v4MtxCol3 = m_writer.DeclLocale( cuT( "l_v4MtxCol3" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_offset += l_mult;
				auto l_v4MtxCol4 = m_writer.DeclLocale( cuT( "l_v4MtxCol4" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_offset += l_mult;
				l_lightReturn.m_transform() = mat4( l_v4MtxCol1, l_v4MtxCol2, l_v4MtxCol3, l_v4MtxCol4 );
			}

			m_writer.Return( l_lightReturn );
		};
		m_writer.ImplementFunction< DirectionalLight >( cuT( "GetDirectionalLight" ), l_get, Int( &m_writer, cuT( "p_index" ) ) );
	}

	void LightingModel::Declare_GetPointLight()
	{
		auto l_get = [this]( Int const & p_index )
		{
			PointLight l_lightReturn = m_writer.DeclLocale< PointLight >( cuT( "l_lightReturn" ) );
			l_lightReturn.m_lightBase() = GetBaseLight( p_index );

			if ( m_writer.HasTexelFetch() )
			{
				if ( m_writer.HasTextureBuffers() )
				{
					auto c3d_sLights = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.DeclLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					auto l_v4PosIndex = m_writer.DeclLocale( cuT( "l_v4PosIndex" ), texelFetch( c3d_sLights, l_offset++ ) );
					l_lightReturn.m_position() = l_v4PosIndex.rgb();
					l_lightReturn.m_index() = m_writer.Cast< Int >( l_v4PosIndex.a() );
					l_lightReturn.m_attenuation() = texelFetch( c3d_sLights, l_offset++ ).rgb();
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.DeclLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					auto l_v4PosIndex = m_writer.DeclLocale( cuT( "l_v4PosIndex" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					l_lightReturn.m_position() = l_v4PosIndex.rgb();
					l_lightReturn.m_index() = m_writer.Cast< Int >( l_v4PosIndex.a() );
					l_lightReturn.m_attenuation() = texelFetch( c3d_sLights, l_offset++, 0 ).rgb();
				}
			}
			else
			{
				auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
				auto l_factor = m_writer.DeclLocale( cuT( "l_factor" ), Float( LightComponentsOffset ) * p_index );
				auto l_decal = m_writer.DeclLocale( cuT( "l_decal" ), 0.0005_f );
				auto l_mult = m_writer.DeclLocale( cuT( "l_mult" ), 0.001_f );
				auto l_offset = m_writer.DeclLocale( cuT( "l_offset" ), l_mult * Float( BaseLightComponentsCount ) );
				auto l_v4PosIndex = m_writer.DeclLocale( cuT( "l_v4PosIndex" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_lightReturn.m_position() = l_v4PosIndex.rgb();
				l_lightReturn.m_index() = m_writer.Cast< Int >( l_v4PosIndex.a() );
				l_offset += l_mult;
				l_lightReturn.m_attenuation() = texture( c3d_sLights, l_factor + l_offset + l_decal ).rgb();
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
			SpotLight l_lightReturn = m_writer.DeclLocale< SpotLight >( cuT( "l_lightReturn" ) );
			l_lightReturn.m_lightBase() = GetBaseLight( p_index );

			if ( m_writer.HasTexelFetch() )
			{
				if ( m_writer.HasTextureBuffers() )
				{
					auto c3d_sLights = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.DeclLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					auto l_v4PosIndex = m_writer.DeclLocale( cuT( "l_v4PosIndex" ), texelFetch( c3d_sLights, l_offset++ ) );
					l_lightReturn.m_position() = l_v4PosIndex.rgb();
					l_lightReturn.m_index() = m_writer.Cast< Int >( l_v4PosIndex.a() );
					l_lightReturn.m_attenuation() = texelFetch( c3d_sLights, l_offset++ ).rgb();
					l_lightReturn.m_direction() = normalize( texelFetch( c3d_sLights, l_offset++ ).rgb() );
					auto l_v2Spot = m_writer.DeclLocale( cuT( "l_v2Spot" ), texelFetch( c3d_sLights, l_offset++ ).rg() );
					l_lightReturn.m_exponent() = l_v2Spot.x();
					l_lightReturn.m_cutOff() = l_v2Spot.y();
					auto l_v4MtxCol1 = m_writer.DeclLocale( cuT( "l_v4MtxCol1" ), texelFetch( c3d_sLights, l_offset++ ) );
					auto l_v4MtxCol2 = m_writer.DeclLocale( cuT( "l_v4MtxCol2" ), texelFetch( c3d_sLights, l_offset++ ) );
					auto l_v4MtxCol3 = m_writer.DeclLocale( cuT( "l_v4MtxCol3" ), texelFetch( c3d_sLights, l_offset++ ) );
					auto l_v4MtxCol4 = m_writer.DeclLocale( cuT( "l_v4MtxCol4" ), texelFetch( c3d_sLights, l_offset++ ) );
					l_lightReturn.m_transform() = mat4( l_v4MtxCol1, l_v4MtxCol2, l_v4MtxCol3, l_v4MtxCol4 );
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto l_offset = m_writer.DeclLocale( cuT( "l_offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					auto l_v4PosIndex = m_writer.DeclLocale( cuT( "l_v4PosIndex" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					l_lightReturn.m_position() = l_v4PosIndex.rgb();
					l_lightReturn.m_index() = m_writer.Cast< Int >( l_v4PosIndex.a() );
					l_lightReturn.m_attenuation() = texelFetch( c3d_sLights, l_offset++, 0 ).rgb();
					l_lightReturn.m_direction() = normalize( texelFetch( c3d_sLights, l_offset++, 0 ).rgb() );
					auto l_v2Spot = m_writer.DeclLocale( cuT( "l_v2Spot" ), texelFetch( c3d_sLights, l_offset++, 0 ).rg() );
					l_lightReturn.m_exponent() = l_v2Spot.x();
					l_lightReturn.m_cutOff() = l_v2Spot.y();
					auto l_v4MtxCol1 = m_writer.DeclLocale( cuT( "l_v4MtxCol1" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					auto l_v4MtxCol2 = m_writer.DeclLocale( cuT( "l_v4MtxCol2" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					auto l_v4MtxCol3 = m_writer.DeclLocale( cuT( "l_v4MtxCol3" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					auto l_v4MtxCol4 = m_writer.DeclLocale( cuT( "l_v4MtxCol4" ), texelFetch( c3d_sLights, l_offset++, 0 ) );
					l_lightReturn.m_transform() = mat4( l_v4MtxCol1, l_v4MtxCol2, l_v4MtxCol3, l_v4MtxCol4 );
					l_lightReturn.m_index() = m_writer.Cast< Int >( texelFetch( c3d_sLights, l_offset++, 0 ).r() );
				}
			}
			else
			{
				auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
				auto l_factor = m_writer.DeclLocale( cuT( "l_factor" ), Float( LightComponentsOffset ) * p_index );
				auto l_decal = m_writer.DeclLocale( cuT( "l_decal" ), 0.0005_f );
				auto l_mult = m_writer.DeclLocale( cuT( "l_mult" ), 0.001_f );
				auto l_offset = m_writer.DeclLocale( cuT( "l_offset" ), l_mult * Float( BaseLightComponentsCount ) );
				auto l_v4PosIndex = m_writer.DeclLocale( cuT( "l_v4PosIndex" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_lightReturn.m_position() = l_v4PosIndex.rgb();
				l_lightReturn.m_index() = m_writer.Cast< Int >( l_v4PosIndex.a() );
				l_offset += l_mult;
				l_lightReturn.m_attenuation() = texture( c3d_sLights, l_factor + l_offset + l_decal ).rgb();
				l_offset += l_mult;
				l_lightReturn.m_direction() = normalize( texture( c3d_sLights, l_factor + l_offset + l_decal ).rgb() );
				l_offset += l_mult;
				auto l_v2Spot = m_writer.DeclLocale( cuT( "l_v2Spot" ), texture( c3d_sLights, l_factor + l_offset + l_decal ).rg() );
				l_lightReturn.m_exponent() = l_v2Spot.x();
				l_lightReturn.m_cutOff() = l_v2Spot.y();
				auto l_v4MtxCol1 = m_writer.DeclLocale( cuT( "l_v4MtxCol1" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_offset += l_mult;
				auto l_v4MtxCol2 = m_writer.DeclLocale( cuT( "l_v4MtxCol2" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_offset += l_mult;
				auto l_v4MtxCol3 = m_writer.DeclLocale( cuT( "l_v4MtxCol3" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_offset += l_mult;
				auto l_v4MtxCol4 = m_writer.DeclLocale( cuT( "l_v4MtxCol4" ), texture( c3d_sLights, l_factor + l_offset + l_decal ) );
				l_offset += l_mult;
				l_lightReturn.m_transform() = mat4( l_v4MtxCol1, l_v4MtxCol2, l_v4MtxCol3, l_v4MtxCol4 );
				l_lightReturn.m_index() = m_writer.Cast< Int >( texture( c3d_sLights, l_offset + l_decal ).r() );
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
}
