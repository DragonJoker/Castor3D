#include "GlslLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	//***********************************************************************************************

	String ParamToString( String & p_sep, FragmentInput const & p_value )
	{
		StringStream result;
		result << ParamToString( p_sep, p_value.m_v3Vertex );
		result << ParamToString( p_sep, p_value.m_v3Normal );
		return result.str();
	}

	String ParamToString( String & p_sep, OutputComponents const & p_value )
	{
		StringStream result;
		result << ParamToString( p_sep, p_value.m_v3Diffuse );
		result << ParamToString( p_sep, p_value.m_v3Specular );
		return result.str();
	}

	String ToString( FragmentInput const & p_value )
	{
		StringStream result;
		result << ToString( p_value.m_v3Vertex ) << ", ";
		result << ToString( p_value.m_v3Normal );
		return result.str();
	}

	String ToString( OutputComponents const & p_value )
	{
		StringStream result;
		result << ToString( p_value.m_v3Diffuse ) << ", ";
		result << ToString( p_value.m_v3Specular );
		return result.str();
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
		Struct lightDecl = m_writer.GetStruct( cuT( "Light" ) );
		lightDecl.DeclMember< Vec3 >( cuT( "m_colour" ) );
		lightDecl.DeclMember< Vec2 >( cuT( "m_intensity" ) );
		lightDecl.End();
	}

	void LightingModel::Declare_DirectionalLight()
	{
		Struct lightDecl = m_writer.GetStruct( cuT( "DirectionalLight" ) );
		lightDecl.DeclMember< Light >( cuT( "m_lightBase" ) );
		lightDecl.DeclMember< Vec3 >( cuT( "m_direction" ) );
		lightDecl.DeclMember< Mat4 >( cuT( "m_transform" ) );
		lightDecl.End();
	}

	void LightingModel::Declare_PointLight()
	{
		Struct lightDecl = m_writer.GetStruct( cuT( "PointLight" ) );
		lightDecl.DeclMember< Light >( cuT( "m_lightBase" ) );
		lightDecl.DeclMember< Vec3 >( cuT( "m_position" ) );
		lightDecl.DeclMember< Vec3 >( cuT( "m_attenuation" ) );
		lightDecl.DeclMember< Int >( cuT( "m_index" ) );
		lightDecl.End();
	}

	void LightingModel::Declare_SpotLight()
	{
		Struct lightDecl = m_writer.GetStruct( cuT( "SpotLight" ) );
		lightDecl.DeclMember< Light >( cuT( "m_lightBase" ) );
		lightDecl.DeclMember< Vec3 >( cuT( "m_position" ) );
		lightDecl.DeclMember< Vec3 >( cuT( "m_attenuation" ) );
		lightDecl.DeclMember< Int >( cuT( "m_index" ) );
		lightDecl.DeclMember< Vec3 >( cuT( "m_direction" ) );
		lightDecl.DeclMember< Float >( cuT( "m_exponent" ) );
		lightDecl.DeclMember< Float >( cuT( "m_cutOff" ) );
		lightDecl.DeclMember< Mat4 >( cuT( "m_transform" ) );
		lightDecl.End();
	}

	void LightingModel::Declare_GetBaseLight()
	{
		auto get = [this]( Int const & p_index )
		{
			auto lightReturn = m_writer.DeclLocale< Light >( cuT( "lightReturn" ) );

			if ( m_writer.HasTexelFetch() )
			{
				if ( m_writer.HasTextureBuffers() )
				{
					auto c3d_sLights = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
					auto offset = m_writer.DeclLocale( cuT( "offset" ), p_index * Int( MaxLightComponentsCount ) );
					lightReturn.m_colour() = texelFetch( c3d_sLights, offset++ ).rgb();
					lightReturn.m_intensity() = texelFetch( c3d_sLights, offset++ ).rg();
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto offset = m_writer.DeclLocale( cuT( "offset" ), p_index * Int( MaxLightComponentsCount ) );
					lightReturn.m_colour() = texelFetch( c3d_sLights, offset++, 0 ).rgb();
					lightReturn.m_intensity() = texelFetch( c3d_sLights, offset++, 0 ).rg();
				}
			}
			else
			{
				auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
				auto factor = m_writer.DeclLocale( cuT( "factor" ), p_index * Int( LightComponentsOffset ) );
				auto offset = m_writer.DeclLocale( cuT( "offset" ), 0.0_f );
				auto decal = m_writer.DeclLocale( cuT( "decal" ), 0.0005_f );
				auto mult = m_writer.DeclLocale( cuT( "mult" ), 0.001_f );
				lightReturn.m_colour() = texture( c3d_sLights, factor + offset + decal ).rgb();
				offset += mult;
				lightReturn.m_intensity() = texture( c3d_sLights, factor + offset + decal ).rg();
				offset += mult;
			}

			m_writer.Return( lightReturn );
		};
		m_writer.ImplementFunction< Light >( cuT( "GetBaseLight" ), get, Int( &m_writer, cuT( "p_index" ) ) );
	}

	void LightingModel::Declare_GetDirectionalLight()
	{
		auto get = [this]( Int const & p_index )
		{
			DirectionalLight lightReturn = m_writer.DeclLocale< DirectionalLight >( cuT( "lightReturn" ) );
			lightReturn.m_lightBase() = GetBaseLight( p_index );

			if ( m_writer.HasTexelFetch() )
			{
				if ( m_writer.HasTextureBuffers() )
				{
					auto c3d_sLights = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
					auto offset = m_writer.DeclLocale( cuT( "offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					lightReturn.m_direction() = texelFetch( c3d_sLights, offset++ ).rgb();
					auto v4MtxCol1 = m_writer.DeclLocale( cuT( "v4MtxCol1" ), texelFetch( c3d_sLights, offset++ ) );
					auto v4MtxCol2 = m_writer.DeclLocale( cuT( "v4MtxCol2" ), texelFetch( c3d_sLights, offset++ ) );
					auto v4MtxCol3 = m_writer.DeclLocale( cuT( "v4MtxCol3" ), texelFetch( c3d_sLights, offset++ ) );
					auto v4MtxCol4 = m_writer.DeclLocale( cuT( "v4MtxCol4" ), texelFetch( c3d_sLights, offset++ ) );
					lightReturn.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto offset = m_writer.DeclLocale( cuT( "offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					lightReturn.m_direction() = texelFetch( c3d_sLights, offset++, 0 ).rgb();
					auto v4MtxCol1 = m_writer.DeclLocale( cuT( "v4MtxCol1" ), texelFetch( c3d_sLights, offset++, 0 ) );
					auto v4MtxCol2 = m_writer.DeclLocale( cuT( "v4MtxCol2" ), texelFetch( c3d_sLights, offset++, 0 ) );
					auto v4MtxCol3 = m_writer.DeclLocale( cuT( "v4MtxCol3" ), texelFetch( c3d_sLights, offset++, 0 ) );
					auto v4MtxCol4 = m_writer.DeclLocale( cuT( "v4MtxCol4" ), texelFetch( c3d_sLights, offset++, 0 ) );
					lightReturn.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
				}
			}
			else
			{
				auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
				auto factor = m_writer.DeclLocale( cuT( "factor" ), Float( LightComponentsOffset ) * p_index );
				auto decal = m_writer.DeclLocale( cuT( "decal" ), 0.0005_f );
				auto mult = m_writer.DeclLocale( cuT( "mult" ), 0.001_f );
				auto offset = m_writer.DeclLocale( cuT( "offset" ), mult * Float( BaseLightComponentsCount ) );
				lightReturn.m_direction() = texture( c3d_sLights, factor + offset + decal ).rgb();
				offset += mult;
				auto v4MtxCol1 = m_writer.DeclLocale( cuT( "v4MtxCol1" ), texture( c3d_sLights, factor + offset + decal ) );
				offset += mult;
				auto v4MtxCol2 = m_writer.DeclLocale( cuT( "v4MtxCol2" ), texture( c3d_sLights, factor + offset + decal ) );
				offset += mult;
				auto v4MtxCol3 = m_writer.DeclLocale( cuT( "v4MtxCol3" ), texture( c3d_sLights, factor + offset + decal ) );
				offset += mult;
				auto v4MtxCol4 = m_writer.DeclLocale( cuT( "v4MtxCol4" ), texture( c3d_sLights, factor + offset + decal ) );
				offset += mult;
				lightReturn.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
			}

			m_writer.Return( lightReturn );
		};
		m_writer.ImplementFunction< DirectionalLight >( cuT( "GetDirectionalLight" ), get, Int( &m_writer, cuT( "p_index" ) ) );
	}

	void LightingModel::Declare_GetPointLight()
	{
		auto get = [this]( Int const & p_index )
		{
			PointLight lightReturn = m_writer.DeclLocale< PointLight >( cuT( "lightReturn" ) );
			lightReturn.m_lightBase() = GetBaseLight( p_index );

			if ( m_writer.HasTexelFetch() )
			{
				if ( m_writer.HasTextureBuffers() )
				{
					auto c3d_sLights = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
					auto offset = m_writer.DeclLocale( cuT( "offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					auto v4PosIndex = m_writer.DeclLocale( cuT( "v4PosIndex" ), texelFetch( c3d_sLights, offset++ ) );
					lightReturn.m_position() = v4PosIndex.rgb();
					lightReturn.m_index() = m_writer.Cast< Int >( v4PosIndex.a() );
					lightReturn.m_attenuation() = texelFetch( c3d_sLights, offset++ ).rgb();
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto offset = m_writer.DeclLocale( cuT( "offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					auto v4PosIndex = m_writer.DeclLocale( cuT( "v4PosIndex" ), texelFetch( c3d_sLights, offset++, 0 ) );
					lightReturn.m_position() = v4PosIndex.rgb();
					lightReturn.m_index() = m_writer.Cast< Int >( v4PosIndex.a() );
					lightReturn.m_attenuation() = texelFetch( c3d_sLights, offset++, 0 ).rgb();
				}
			}
			else
			{
				auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
				auto factor = m_writer.DeclLocale( cuT( "factor" ), Float( LightComponentsOffset ) * p_index );
				auto decal = m_writer.DeclLocale( cuT( "decal" ), 0.0005_f );
				auto mult = m_writer.DeclLocale( cuT( "mult" ), 0.001_f );
				auto offset = m_writer.DeclLocale( cuT( "offset" ), mult * Float( BaseLightComponentsCount ) );
				auto v4PosIndex = m_writer.DeclLocale( cuT( "v4PosIndex" ), texture( c3d_sLights, factor + offset + decal ) );
				lightReturn.m_position() = v4PosIndex.rgb();
				lightReturn.m_index() = m_writer.Cast< Int >( v4PosIndex.a() );
				offset += mult;
				lightReturn.m_attenuation() = texture( c3d_sLights, factor + offset + decal ).rgb();
				offset += mult;
			}

			m_writer.Return( lightReturn );
		};
		m_writer.ImplementFunction< PointLight >( cuT( "GetPointLight" ), get, Int( &m_writer, cuT( "p_index" ) ) );
	}

	void LightingModel::Declare_GetSpotLight()
	{
		auto get = [this]( Int const & p_index )
		{
			SpotLight lightReturn = m_writer.DeclLocale< SpotLight >( cuT( "lightReturn" ) );
			lightReturn.m_lightBase() = GetBaseLight( p_index );

			if ( m_writer.HasTexelFetch() )
			{
				if ( m_writer.HasTextureBuffers() )
				{
					auto c3d_sLights = m_writer.GetBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
					auto offset = m_writer.DeclLocale( cuT( "offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					auto v4PosIndex = m_writer.DeclLocale( cuT( "v4PosIndex" ), texelFetch( c3d_sLights, offset++ ) );
					lightReturn.m_position() = v4PosIndex.rgb();
					lightReturn.m_index() = m_writer.Cast< Int >( v4PosIndex.a() );
					lightReturn.m_attenuation() = texelFetch( c3d_sLights, offset++ ).rgb();
					lightReturn.m_direction() = normalize( texelFetch( c3d_sLights, offset++ ).rgb() );
					auto v2Spot = m_writer.DeclLocale( cuT( "v2Spot" ), texelFetch( c3d_sLights, offset++ ).rg() );
					lightReturn.m_exponent() = v2Spot.x();
					lightReturn.m_cutOff() = v2Spot.y();
					auto v4MtxCol1 = m_writer.DeclLocale( cuT( "v4MtxCol1" ), texelFetch( c3d_sLights, offset++ ) );
					auto v4MtxCol2 = m_writer.DeclLocale( cuT( "v4MtxCol2" ), texelFetch( c3d_sLights, offset++ ) );
					auto v4MtxCol3 = m_writer.DeclLocale( cuT( "v4MtxCol3" ), texelFetch( c3d_sLights, offset++ ) );
					auto v4MtxCol4 = m_writer.DeclLocale( cuT( "v4MtxCol4" ), texelFetch( c3d_sLights, offset++ ) );
					lightReturn.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
				}
				else
				{
					auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto offset = m_writer.DeclLocale( cuT( "offset" ), p_index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
					auto v4PosIndex = m_writer.DeclLocale( cuT( "v4PosIndex" ), texelFetch( c3d_sLights, offset++, 0 ) );
					lightReturn.m_position() = v4PosIndex.rgb();
					lightReturn.m_index() = m_writer.Cast< Int >( v4PosIndex.a() );
					lightReturn.m_attenuation() = texelFetch( c3d_sLights, offset++, 0 ).rgb();
					lightReturn.m_direction() = normalize( texelFetch( c3d_sLights, offset++, 0 ).rgb() );
					auto v2Spot = m_writer.DeclLocale( cuT( "v2Spot" ), texelFetch( c3d_sLights, offset++, 0 ).rg() );
					lightReturn.m_exponent() = v2Spot.x();
					lightReturn.m_cutOff() = v2Spot.y();
					auto v4MtxCol1 = m_writer.DeclLocale( cuT( "v4MtxCol1" ), texelFetch( c3d_sLights, offset++, 0 ) );
					auto v4MtxCol2 = m_writer.DeclLocale( cuT( "v4MtxCol2" ), texelFetch( c3d_sLights, offset++, 0 ) );
					auto v4MtxCol3 = m_writer.DeclLocale( cuT( "v4MtxCol3" ), texelFetch( c3d_sLights, offset++, 0 ) );
					auto v4MtxCol4 = m_writer.DeclLocale( cuT( "v4MtxCol4" ), texelFetch( c3d_sLights, offset++, 0 ) );
					lightReturn.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
					lightReturn.m_index() = m_writer.Cast< Int >( texelFetch( c3d_sLights, offset++, 0 ).r() );
				}
			}
			else
			{
				auto c3d_sLights = m_writer.GetBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
				auto factor = m_writer.DeclLocale( cuT( "factor" ), Float( LightComponentsOffset ) * p_index );
				auto decal = m_writer.DeclLocale( cuT( "decal" ), 0.0005_f );
				auto mult = m_writer.DeclLocale( cuT( "mult" ), 0.001_f );
				auto offset = m_writer.DeclLocale( cuT( "offset" ), mult * Float( BaseLightComponentsCount ) );
				auto v4PosIndex = m_writer.DeclLocale( cuT( "v4PosIndex" ), texture( c3d_sLights, factor + offset + decal ) );
				lightReturn.m_position() = v4PosIndex.rgb();
				lightReturn.m_index() = m_writer.Cast< Int >( v4PosIndex.a() );
				offset += mult;
				lightReturn.m_attenuation() = texture( c3d_sLights, factor + offset + decal ).rgb();
				offset += mult;
				lightReturn.m_direction() = normalize( texture( c3d_sLights, factor + offset + decal ).rgb() );
				offset += mult;
				auto v2Spot = m_writer.DeclLocale( cuT( "v2Spot" ), texture( c3d_sLights, factor + offset + decal ).rg() );
				lightReturn.m_exponent() = v2Spot.x();
				lightReturn.m_cutOff() = v2Spot.y();
				auto v4MtxCol1 = m_writer.DeclLocale( cuT( "v4MtxCol1" ), texture( c3d_sLights, factor + offset + decal ) );
				offset += mult;
				auto v4MtxCol2 = m_writer.DeclLocale( cuT( "v4MtxCol2" ), texture( c3d_sLights, factor + offset + decal ) );
				offset += mult;
				auto v4MtxCol3 = m_writer.DeclLocale( cuT( "v4MtxCol3" ), texture( c3d_sLights, factor + offset + decal ) );
				offset += mult;
				auto v4MtxCol4 = m_writer.DeclLocale( cuT( "v4MtxCol4" ), texture( c3d_sLights, factor + offset + decal ) );
				offset += mult;
				lightReturn.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
				lightReturn.m_index() = m_writer.Cast< Int >( texture( c3d_sLights, offset + decal ).r() );
				offset += mult;
			}

			m_writer.Return( lightReturn );
		};
		m_writer.ImplementFunction< SpotLight >( cuT( "GetSpotLight" ), get, Int( &m_writer, cuT( "p_index" ) ) );
	}

	Light LightingModel::GetBaseLight( Type const & p_value )
	{
		return WriteFunctionCall< Light >( &m_writer, cuT( "GetBaseLight" ), p_value );
	}
}
