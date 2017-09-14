#include "GlslLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"
#include "GlslLight.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		//***********************************************************************************************

		String paramToString( String & p_sep, FragmentInput const & p_value )
		{
			StringStream result;
			result << paramToString( p_sep, p_value.m_vertex );
			result << paramToString( p_sep, p_value.m_normal );
			return result.str();
		}

		String paramToString( String & p_sep, OutputComponents const & p_value )
		{
			StringStream result;
			result << paramToString( p_sep, p_value.m_diffuse );
			result << paramToString( p_sep, p_value.m_specular );
			return result.str();
		}

		String toString( FragmentInput const & p_value )
		{
			StringStream result;
			result << toString( p_value.m_vertex ) << ", ";
			result << toString( p_value.m_normal );
			return result.str();
		}

		String toString( OutputComponents const & p_value )
		{
			StringStream result;
			result << toString( p_value.m_diffuse ) << ", ";
			result << toString( p_value.m_specular );
			return result.str();
		}

		//***********************************************************************************************

		FragmentInput::FragmentInput( GlslWriter & p_writer )
			: m_vertex{ &p_writer, cuT( "inVertex" ) }
			, m_normal{ &p_writer, cuT( "inNormal" ) }
		{
		}

		FragmentInput::FragmentInput( InVec3 const & p_v3Vertex
			, InVec3 const & p_v3Normal )
			: m_vertex{ p_v3Vertex }
			, m_normal{ p_v3Normal }
		{
		}

		//***********************************************************************************************

		OutputComponents::OutputComponents( GlslWriter & p_writer )
			: m_diffuse{ &p_writer, cuT( "outDiffuse" ) }
			, m_specular{ &p_writer, cuT( "outSpecular" ) }
		{
		}

		OutputComponents::OutputComponents( InOutVec3 const & p_v3Diffuse
			, InOutVec3 const & p_v3Specular )
			: m_diffuse{ p_v3Diffuse }
			, m_specular{ p_v3Specular }
		{
		}

		//***********************************************************************************************

		LightingModel::LightingModel( ShadowType p_shadows, GlslWriter & p_writer )
			: m_shadows{ p_shadows }
			, m_writer{ p_writer }
			, m_shadowModel{ std::make_shared< Shadow >( p_writer ) }
		{
		}

		void LightingModel::declareModel()
		{
			if ( m_shadows != ShadowType::eNone )
			{
				m_shadowModel->declare( m_shadows );
			}

			doDeclareLight();
			doDeclareDirectionalLight();
			doDeclarePointLight();
			doDeclareSpotLight();
			doDeclareGetBaseLight();
			doDeclareGetDirectionalLight();
			doDeclareGetPointLight();
			doDeclareGetSpotLight();
			doDeclareModel();
			doDeclareComputeDirectionalLight();
			doDeclareComputePointLight();
			doDeclareComputeSpotLight();
			doDeclareComputeDirectionalLightBackLit();
			doDeclareComputePointLightBackLit();
			doDeclareComputeSpotLightBackLit();
		}

		void LightingModel::declareDirectionalModel()
		{
			if ( m_shadows != ShadowType::eNone )
			{
				m_shadowModel->declareDirectional( m_shadows );
			}

			doDeclareLight();
			doDeclareDirectionalLight();
			doDeclareModel();
			doDeclareComputeDirectionalLight();
			doDeclareComputeDirectionalLightBackLit();
		}

		void LightingModel::declarePointModel()
		{
			if ( m_shadows != ShadowType::eNone )
			{
				m_shadowModel->declarePoint( m_shadows );
			}

			doDeclareLight();
			doDeclarePointLight();
			doDeclareModel();
			doDeclareComputeOnePointLight();
			doDeclareComputeOnePointLightBackLit();
		}

		void LightingModel::declareSpotModel()
		{
			if ( m_shadows != ShadowType::eNone )
			{
				m_shadowModel->declareSpot( m_shadows );
			}

			doDeclareLight();
			doDeclareSpotLight();
			doDeclareModel();
			doDeclareComputeOneSpotLight();
			doDeclareComputeOneSpotLightBackLit();
		}

		DirectionalLight LightingModel::getDirectionalLight( Int const & index )
		{
			return m_getDirectionalLight( index );
		}

		PointLight LightingModel::getPointLight( Int const & index )
		{
			return m_getPointLight( index );
		}

		SpotLight LightingModel::getSpotLight( Int const & index )
		{
			return m_getSpotLight( index );
		}

		void LightingModel::doDeclareLight()
		{
			Struct lightDecl = m_writer.getStruct( cuT( "Light" ) );
			lightDecl.declMember< Vec3 >( cuT( "m_colour" ) );
			lightDecl.declMember< Vec2 >( cuT( "m_intensity" ) );
			lightDecl.end();
		}

		void LightingModel::doDeclareDirectionalLight()
		{
			Struct lightDecl = m_writer.getStruct( cuT( "DirectionalLight" ) );
			lightDecl.declMember< Light >( cuT( "m_lightBase" ) );
			lightDecl.declMember< Vec3 >( cuT( "m_direction" ) );
			lightDecl.declMember< Mat4 >( cuT( "m_transform" ) );
			lightDecl.end();
		}

		void LightingModel::doDeclarePointLight()
		{
			Struct lightDecl = m_writer.getStruct( cuT( "PointLight" ) );
			lightDecl.declMember< Light >( cuT( "m_lightBase" ) );
			lightDecl.declMember< Vec3 >( cuT( "m_position" ) );
			lightDecl.declMember< Vec3 >( cuT( "m_attenuation" ) );
			lightDecl.declMember< Float >( cuT( "m_farPlane" ) );
			lightDecl.declMember< Int >( cuT( "m_index" ) );
			lightDecl.end();
		}

		void LightingModel::doDeclareSpotLight()
		{
			Struct lightDecl = m_writer.getStruct( cuT( "SpotLight" ) );
			lightDecl.declMember< Light >( cuT( "m_lightBase" ) );
			lightDecl.declMember< Vec3 >( cuT( "m_position" ) );
			lightDecl.declMember< Vec3 >( cuT( "m_attenuation" ) );
			lightDecl.declMember< Float >( cuT( "m_farPlane" ) );
			lightDecl.declMember< Int >( cuT( "m_index" ) );
			lightDecl.declMember< Vec3 >( cuT( "m_direction" ) );
			lightDecl.declMember< Float >( cuT( "m_exponent" ) );
			lightDecl.declMember< Float >( cuT( "m_cutOff" ) );
			lightDecl.declMember< Mat4 >( cuT( "m_transform" ) );
			lightDecl.end();
		}

		void LightingModel::doDeclareGetBaseLight()
		{
			auto get = [this]( Int const & index )
			{
				auto result = m_writer.declLocale< Light >( cuT( "result" ) );

				if ( m_writer.hasTexelFetch() )
				{
					if ( m_writer.hasTextureBuffers() )
					{
						auto c3d_sLights = m_writer.getBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
						auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) );
						result.m_colour() = texelFetch( c3d_sLights, offset++ ).rgb();
						result.m_intensity() = texelFetch( c3d_sLights, offset++ ).rg();
					}
					else
					{
						auto c3d_sLights = m_writer.getBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
						auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) );
						result.m_colour() = texelFetch( c3d_sLights, offset++, 0 ).rgb();
						result.m_intensity() = texelFetch( c3d_sLights, offset++, 0 ).rg();
					}
				}
				else
				{
					auto c3d_sLights = m_writer.getBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
					auto factor = m_writer.declLocale( cuT( "factor" ), index * Int( LightComponentsOffset ) );
					auto offset = m_writer.declLocale( cuT( "offset" ), 0.0_f );
					auto decal = m_writer.declLocale( cuT( "decal" ), 0.0005_f );
					auto mult = m_writer.declLocale( cuT( "mult" ), 0.001_f );
					result.m_colour() = texture( c3d_sLights, factor + offset + decal ).rgb();
					offset += mult;
					result.m_intensity() = texture( c3d_sLights, factor + offset + decal ).rg();
					offset += mult;
				}

				m_writer.returnStmt( result );
			};
			m_writer.implementFunction< Light >( cuT( "getBaseLight" ), get, Int( &m_writer, cuT( "index" ) ) );
		}

		void LightingModel::doDeclareGetDirectionalLight()
		{
			m_getDirectionalLight = m_writer.implementFunction< DirectionalLight >( cuT( "getDirectionalLight" )
				, [this]( Int const & index )
				{
					DirectionalLight result = m_writer.declLocale< DirectionalLight >( cuT( "result" ) );
					result.m_lightBase() = getBaseLight( index );

					if ( m_writer.hasTexelFetch() )
					{
						if ( m_writer.hasTextureBuffers() )
						{
							auto c3d_sLights = m_writer.getBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
							auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
							result.m_direction() = texelFetch( c3d_sLights, offset++ ).rgb();
							auto v4MtxCol1 = m_writer.declLocale( cuT( "v4MtxCol1" ), texelFetch( c3d_sLights, offset++ ) );
							auto v4MtxCol2 = m_writer.declLocale( cuT( "v4MtxCol2" ), texelFetch( c3d_sLights, offset++ ) );
							auto v4MtxCol3 = m_writer.declLocale( cuT( "v4MtxCol3" ), texelFetch( c3d_sLights, offset++ ) );
							auto v4MtxCol4 = m_writer.declLocale( cuT( "v4MtxCol4" ), texelFetch( c3d_sLights, offset++ ) );
							result.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
						}
						else
						{
							auto c3d_sLights = m_writer.getBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
							auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
							result.m_direction() = texelFetch( c3d_sLights, offset++, 0 ).rgb();
							auto v4MtxCol1 = m_writer.declLocale( cuT( "v4MtxCol1" ), texelFetch( c3d_sLights, offset++, 0 ) );
							auto v4MtxCol2 = m_writer.declLocale( cuT( "v4MtxCol2" ), texelFetch( c3d_sLights, offset++, 0 ) );
							auto v4MtxCol3 = m_writer.declLocale( cuT( "v4MtxCol3" ), texelFetch( c3d_sLights, offset++, 0 ) );
							auto v4MtxCol4 = m_writer.declLocale( cuT( "v4MtxCol4" ), texelFetch( c3d_sLights, offset++, 0 ) );
							result.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
						}
					}
					else
					{
						auto c3d_sLights = m_writer.getBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
						auto factor = m_writer.declLocale( cuT( "factor" ), Float( LightComponentsOffset ) * index );
						auto decal = m_writer.declLocale( cuT( "decal" ), 0.0005_f );
						auto mult = m_writer.declLocale( cuT( "mult" ), 0.001_f );
						auto offset = m_writer.declLocale( cuT( "offset" ), mult * Float( BaseLightComponentsCount ) );
						result.m_direction() = texture( c3d_sLights, factor + offset + decal ).rgb();
						offset += mult;
						auto v4MtxCol1 = m_writer.declLocale( cuT( "v4MtxCol1" ), texture( c3d_sLights, factor + offset + decal ) );
						offset += mult;
						auto v4MtxCol2 = m_writer.declLocale( cuT( "v4MtxCol2" ), texture( c3d_sLights, factor + offset + decal ) );
						offset += mult;
						auto v4MtxCol3 = m_writer.declLocale( cuT( "v4MtxCol3" ), texture( c3d_sLights, factor + offset + decal ) );
						offset += mult;
						auto v4MtxCol4 = m_writer.declLocale( cuT( "v4MtxCol4" ), texture( c3d_sLights, factor + offset + decal ) );
						offset += mult;
						result.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
					}

					m_writer.returnStmt( result );
				}
				, InInt{ &m_writer, cuT( "index" ) } );
		}

		void LightingModel::doDeclareGetPointLight()
		{
			m_getPointLight = m_writer.implementFunction< PointLight >( cuT( "getPointLight" )
				, [this]( Int const & index )
				{
					PointLight result = m_writer.declLocale< PointLight >( cuT( "result" ) );
					result.m_lightBase() = getBaseLight( index );

					if ( m_writer.hasTexelFetch() )
					{
						if ( m_writer.hasTextureBuffers() )
						{
							auto c3d_sLights = m_writer.getBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
							auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
							auto v4PosIndex = m_writer.declLocale( cuT( "v4PosIndex" ), texelFetch( c3d_sLights, offset++ ) );
							result.m_position() = v4PosIndex.rgb();
							result.m_index() = m_writer.cast< Int >( v4PosIndex.a() );
							auto v4AttFarPlane = m_writer.declLocale( cuT( "v4AttFarPlane" ), texelFetch( c3d_sLights, offset++ ) );
							result.m_attenuation() = v4AttFarPlane.rgb();
							result.m_farPlane() = v4AttFarPlane.a();
						}
						else
						{
							auto c3d_sLights = m_writer.getBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
							auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
							auto v4PosIndex = m_writer.declLocale( cuT( "v4PosIndex" ), texelFetch( c3d_sLights, offset++, 0 ) );
							result.m_position() = v4PosIndex.rgb();
							result.m_index() = m_writer.cast< Int >( v4PosIndex.a() );
							auto v4AttFarPlane = m_writer.declLocale( cuT( "v4AttFarPlane" ), texelFetch( c3d_sLights, offset++, 0 ) );
							result.m_attenuation() = v4AttFarPlane.rgb();
							result.m_farPlane() = v4AttFarPlane.a();
						}
					}
					else
					{
						auto c3d_sLights = m_writer.getBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
						auto factor = m_writer.declLocale( cuT( "factor" ), Float( LightComponentsOffset ) * index );
						auto decal = m_writer.declLocale( cuT( "decal" ), 0.0005_f );
						auto mult = m_writer.declLocale( cuT( "mult" ), 0.001_f );
						auto offset = m_writer.declLocale( cuT( "offset" ), mult * Float( BaseLightComponentsCount ) );
						auto v4PosIndex = m_writer.declLocale( cuT( "v4PosIndex" ), texture( c3d_sLights, factor + offset + decal ) );
						result.m_position() = v4PosIndex.rgb();
						result.m_index() = m_writer.cast< Int >( v4PosIndex.a() );
						offset += mult;
						auto v4AttFarPlane = m_writer.declLocale( cuT( "v4AttFarPlane" ), texture( c3d_sLights, factor + offset + decal ) );
						result.m_attenuation() = v4AttFarPlane.rgb();
						result.m_farPlane() = v4AttFarPlane.a();
						offset += mult;
					}

					m_writer.returnStmt( result );
				}
				, InInt{ &m_writer, cuT( "index" ) } );
		}

		void LightingModel::doDeclareGetSpotLight()
		{
			m_getSpotLight = m_writer.implementFunction< SpotLight >( cuT( "getSpotLight" )
				, [this]( Int const & index )
				{
					SpotLight result = m_writer.declLocale< SpotLight >( cuT( "result" ) );
					result.m_lightBase() = getBaseLight( index );

					if ( m_writer.hasTexelFetch() )
					{
						if ( m_writer.hasTextureBuffers() )
						{
							auto c3d_sLights = m_writer.getBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
							auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
							auto v4PosIndex = m_writer.declLocale( cuT( "v4PosIndex" ), texelFetch( c3d_sLights, offset++ ) );
							result.m_position() = v4PosIndex.rgb();
							result.m_index() = m_writer.cast< Int >( v4PosIndex.a() );
							auto v4AttFarPlane = m_writer.declLocale( cuT( "v4AttFarPlane" ), texelFetch( c3d_sLights, offset++ ) );
							result.m_attenuation() = v4AttFarPlane.rgb();
							result.m_farPlane() = v4AttFarPlane.a();
							result.m_direction() = normalize( texelFetch( c3d_sLights, offset++ ).rgb() );
							auto v2Spot = m_writer.declLocale( cuT( "v2Spot" ), texelFetch( c3d_sLights, offset++ ).rg() );
							result.m_exponent() = v2Spot.x();
							result.m_cutOff() = v2Spot.y();
							auto v4MtxCol1 = m_writer.declLocale( cuT( "v4MtxCol1" ), texelFetch( c3d_sLights, offset++ ) );
							auto v4MtxCol2 = m_writer.declLocale( cuT( "v4MtxCol2" ), texelFetch( c3d_sLights, offset++ ) );
							auto v4MtxCol3 = m_writer.declLocale( cuT( "v4MtxCol3" ), texelFetch( c3d_sLights, offset++ ) );
							auto v4MtxCol4 = m_writer.declLocale( cuT( "v4MtxCol4" ), texelFetch( c3d_sLights, offset++ ) );
							result.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
						}
						else
						{
							auto c3d_sLights = m_writer.getBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
							auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
							auto v4PosIndex = m_writer.declLocale( cuT( "v4PosIndex" ), texelFetch( c3d_sLights, offset++, 0 ) );
							result.m_position() = v4PosIndex.rgb();
							result.m_index() = m_writer.cast< Int >( v4PosIndex.a() );
							auto v4AttFarPlane = m_writer.declLocale( cuT( "v4AttFarPlane" ), texelFetch( c3d_sLights, offset++, 0 ) );
							result.m_attenuation() = v4AttFarPlane.rgb();
							result.m_farPlane() = v4AttFarPlane.a();
							result.m_direction() = normalize( texelFetch( c3d_sLights, offset++, 0 ).rgb() );
							auto v2Spot = m_writer.declLocale( cuT( "v2Spot" ), texelFetch( c3d_sLights, offset++, 0 ).rg() );
							result.m_exponent() = v2Spot.x();
							result.m_cutOff() = v2Spot.y();
							auto v4MtxCol1 = m_writer.declLocale( cuT( "v4MtxCol1" ), texelFetch( c3d_sLights, offset++, 0 ) );
							auto v4MtxCol2 = m_writer.declLocale( cuT( "v4MtxCol2" ), texelFetch( c3d_sLights, offset++, 0 ) );
							auto v4MtxCol3 = m_writer.declLocale( cuT( "v4MtxCol3" ), texelFetch( c3d_sLights, offset++, 0 ) );
							auto v4MtxCol4 = m_writer.declLocale( cuT( "v4MtxCol4" ), texelFetch( c3d_sLights, offset++, 0 ) );
							result.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
							result.m_index() = m_writer.cast< Int >( texelFetch( c3d_sLights, offset++, 0 ).r() );
						}
					}
					else
					{
						auto c3d_sLights = m_writer.getBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
						auto factor = m_writer.declLocale( cuT( "factor" ), Float( LightComponentsOffset ) * index );
						auto decal = m_writer.declLocale( cuT( "decal" ), 0.0005_f );
						auto mult = m_writer.declLocale( cuT( "mult" ), 0.001_f );
						auto offset = m_writer.declLocale( cuT( "offset" ), mult * Float( BaseLightComponentsCount ) );
						auto v4PosIndex = m_writer.declLocale( cuT( "v4PosIndex" ), texture( c3d_sLights, factor + offset + decal ) );
						result.m_position() = v4PosIndex.rgb();
						result.m_index() = m_writer.cast< Int >( v4PosIndex.a() );
						offset += mult;
						auto v4AttFarPlane = m_writer.declLocale( cuT( "v4AttFarPlane" ), texture( c3d_sLights, factor + offset + decal ) );
						result.m_attenuation() = v4AttFarPlane.rgb();
						result.m_farPlane() = v4AttFarPlane.a();
						offset += mult;
						result.m_direction() = normalize( texture( c3d_sLights, factor + offset + decal ).rgb() );
						offset += mult;
						auto v2Spot = m_writer.declLocale( cuT( "v2Spot" ), texture( c3d_sLights, factor + offset + decal ).rg() );
						result.m_exponent() = v2Spot.x();
						result.m_cutOff() = v2Spot.y();
						auto v4MtxCol1 = m_writer.declLocale( cuT( "v4MtxCol1" ), texture( c3d_sLights, factor + offset + decal ) );
						offset += mult;
						auto v4MtxCol2 = m_writer.declLocale( cuT( "v4MtxCol2" ), texture( c3d_sLights, factor + offset + decal ) );
						offset += mult;
						auto v4MtxCol3 = m_writer.declLocale( cuT( "v4MtxCol3" ), texture( c3d_sLights, factor + offset + decal ) );
						offset += mult;
						auto v4MtxCol4 = m_writer.declLocale( cuT( "v4MtxCol4" ), texture( c3d_sLights, factor + offset + decal ) );
						offset += mult;
						result.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
						result.m_index() = m_writer.cast< Int >( texture( c3d_sLights, offset + decal ).r() );
						offset += mult;
					}

					m_writer.returnStmt( result );
				}
				, InInt{ &m_writer, cuT( "index" ) } );
		}

		Light LightingModel::getBaseLight( Type const & p_value )
		{
			return writeFunctionCall< Light >( &m_writer, cuT( "getBaseLight" ), p_value );
		}
	}
}
