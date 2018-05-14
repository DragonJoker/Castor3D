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

		String paramToString( String & sep, FragmentInput const & value )
		{
			StringStream result{ makeStringStream() };
			result << paramToString( sep, value.m_vertex );
			result << paramToString( sep, value.m_normal );
			return result.str();
		}

		String paramToString( String & sep, OutputComponents const & value )
		{
			StringStream result{ makeStringStream() };
			result << paramToString( sep, value.m_diffuse );
			result << paramToString( sep, value.m_specular );
			return result.str();
		}

		String toString( FragmentInput const & value )
		{
			StringStream result{ makeStringStream() };
			result << toString( value.m_vertex ) << ", ";
			result << toString( value.m_normal );
			return result.str();
		}

		String toString( OutputComponents const & value )
		{
			StringStream result{ makeStringStream() };
			result << toString( value.m_diffuse ) << ", ";
			result << toString( value.m_specular );
			return result.str();
		}

		//***********************************************************************************************

		FragmentInput::FragmentInput( GlslWriter & writer )
			: m_vertex{ &writer, cuT( "inVertex" ) }
			, m_normal{ &writer, cuT( "inNormal" ) }
		{
		}

		FragmentInput::FragmentInput( InVec3 const & v3Vertex
			, InVec3 const & v3Normal )
			: m_vertex{ v3Vertex }
			, m_normal{ v3Normal }
		{
		}

		//***********************************************************************************************

		OutputComponents::OutputComponents( GlslWriter & writer )
			: m_diffuse{ &writer, cuT( "outDiffuse" ) }
			, m_specular{ &writer, cuT( "outSpecular" ) }
		{
		}

		OutputComponents::OutputComponents( InOutVec3 const & v3Diffuse
			, InOutVec3 const & v3Specular )
			: m_diffuse{ v3Diffuse }
			, m_specular{ v3Specular }
		{
		}

		//***********************************************************************************************

		LightingModel::LightingModel( GlslWriter & writer )
			: m_writer{ writer }
			, m_shadowModel{ std::make_shared< Shadow >( writer ) }
		{
		}

		void LightingModel::declareModel( uint32_t & index )
		{
			m_shadowModel->declare( index );
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
		}

		void LightingModel::declareDirectionalModel( ShadowType shadows, uint32_t & index )
		{
			if ( shadows != ShadowType::eNone )
			{
				m_shadowModel->declareDirectional( shadows, index );
			}

			doDeclareLight();
			doDeclareDirectionalLight();
			doDeclareDirectionalLightUbo();
			doDeclareModel();
			doDeclareComputeOneDirectionalLight( shadows );
		}

		void LightingModel::declarePointModel( ShadowType shadows, uint32_t & index )
		{
			if ( shadows != ShadowType::eNone )
			{
				m_shadowModel->declarePoint( shadows, index );
			}

			doDeclareLight();
			doDeclarePointLight();
			doDeclarePointLightUbo();
			doDeclareModel();
			doDeclareComputeOnePointLight( shadows );
		}

		void LightingModel::declareSpotModel( ShadowType shadows, uint32_t & index )
		{
			if ( shadows != ShadowType::eNone )
			{
				m_shadowModel->declareSpot( shadows, index );
			}

			doDeclareLight();
			doDeclareSpotLight();
			doDeclareSpotLightUbo();
			doDeclareModel();
			doDeclareComputeOneSpotLight( shadows );
		}

		DirectionalLight LightingModel::getDirectionalLight( Int const & index )const
		{
			return m_getDirectionalLight( index );
		}

		PointLight LightingModel::getPointLight( Int const & index )const
		{
			return m_getPointLight( index );
		}

		SpotLight LightingModel::getSpotLight( Int const & index )const
		{
			return m_getSpotLight( index );
		}

		void LightingModel::doDeclareLight()
		{
			Struct lightDecl = m_writer.getStruct( cuT( "Light" ) );
			lightDecl.declMember< Vec4 >( cuT( "m_colourIndex" ) );
			lightDecl.declMember< Vec4 >( cuT( "m_intensityFarPlane" ) );
			lightDecl.end();
		}

		void LightingModel::doDeclareDirectionalLight()
		{
			Struct lightDecl = m_writer.getStruct( cuT( "DirectionalLight" ) );
			lightDecl.declMember< Light >( cuT( "m_lightBase" ) );
			lightDecl.declMember< Vec4 >( cuT( "m_direction" ) );
			lightDecl.declMember< Mat4 >( cuT( "m_transform" ) );
			lightDecl.end();
		}

		void LightingModel::doDeclarePointLight()
		{
			Struct lightDecl = m_writer.getStruct( cuT( "PointLight" ) );
			lightDecl.declMember< Light >( cuT( "m_lightBase" ) );
			lightDecl.declMember< Vec4 >( cuT( "m_position" ) );
			lightDecl.declMember< Vec4 >( cuT( "m_attenuation" ) );
			lightDecl.end();
		}

		void LightingModel::doDeclareSpotLight()
		{
			Struct lightDecl = m_writer.getStruct( cuT( "SpotLight" ) );
			lightDecl.declMember< Light >( cuT( "m_lightBase" ) );
			lightDecl.declMember< Vec4 >( cuT( "m_position" ) );
			lightDecl.declMember< Vec4 >( cuT( "m_attenuation" ) );
			lightDecl.declMember< Vec4 >( cuT( "m_direction" ) );
			lightDecl.declMember< Vec4 >( cuT( "m_exponentCutOff" ) );
			lightDecl.declMember< Mat4 >( cuT( "m_transform" ) );
			lightDecl.end();
		}

		void LightingModel::doDeclareDirectionalLightUbo()
		{
			Ubo lightUbo{ m_writer, cuT( "LightUbo" ), UboBindingPoint, 0u };
			lightUbo.declMember< DirectionalLight >( cuT( "c3d_light" ) );
			lightUbo.end();
		}

		void LightingModel::doDeclarePointLightUbo()
		{
			Ubo lightUbo{ m_writer, cuT( "LightUbo" ), UboBindingPoint, 0u };
			lightUbo.declMember< PointLight >( cuT( "c3d_light" ) );
			lightUbo.end();
		}

		void LightingModel::doDeclareSpotLightUbo()
		{
			Ubo lightUbo{ m_writer, cuT( "LightUbo" ), UboBindingPoint, 0u };
			lightUbo.declMember< SpotLight >( cuT( "c3d_light" ) );
			lightUbo.end();
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
						result.m_colourIndex() = texelFetch( c3d_sLights, offset++ );
						result.m_intensityFarPlane() = texelFetch( c3d_sLights, offset++ );
					}
					else
					{
						auto c3d_sLights = m_writer.getBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
						auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) );
						result.m_colourIndex() = texelFetch( c3d_sLights, offset++, 0 );
						result.m_intensityFarPlane() = texelFetch( c3d_sLights, offset++, 0 );
					}
				}
				else
				{
					CASTOR_EXCEPTION( "TBOS are required" );
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
							auto direction = m_writer.declLocale( cuT( "direction" ), texelFetch( c3d_sLights, offset++ ) );
							result.m_direction() = direction.rgb();
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
							auto direction = m_writer.declLocale( cuT( "direction" ), texelFetch( c3d_sLights, offset++, 0 ) );
							result.m_direction() = direction.rgb();
							auto v4MtxCol1 = m_writer.declLocale( cuT( "v4MtxCol1" ), texelFetch( c3d_sLights, offset++, 0 ) );
							auto v4MtxCol2 = m_writer.declLocale( cuT( "v4MtxCol2" ), texelFetch( c3d_sLights, offset++, 0 ) );
							auto v4MtxCol3 = m_writer.declLocale( cuT( "v4MtxCol3" ), texelFetch( c3d_sLights, offset++, 0 ) );
							auto v4MtxCol4 = m_writer.declLocale( cuT( "v4MtxCol4" ), texelFetch( c3d_sLights, offset++, 0 ) );
							result.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
						}
					}
					else
					{
						CASTOR_EXCEPTION( "TBOS are required" );
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
							result.m_position() = texelFetch( c3d_sLights, offset++ ).rgb();
							result.m_attenuation() = texelFetch( c3d_sLights, offset++ ).rgb();
						}
						else
						{
							auto c3d_sLights = m_writer.getBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
							auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
							result.m_position() = texelFetch( c3d_sLights, offset++, 0 ).rgb();
							result.m_attenuation() = texelFetch( c3d_sLights, offset++, 0 ).rgb();
						}
					}
					else
					{
						CASTOR_EXCEPTION( "TBOS are required" );
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
							result.m_position() = texelFetch( c3d_sLights, offset++ ).rgb();
							result.m_attenuation() = texelFetch( c3d_sLights, offset++ ).rgb();
							result.m_direction() = normalize( texelFetch( c3d_sLights, offset++ ).rgb() );
							result.m_exponentCutOff() = texelFetch( c3d_sLights, offset++ );
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
							result.m_position() = texelFetch( c3d_sLights, offset++, 0 ).rgb();
							result.m_attenuation() = texelFetch( c3d_sLights, offset++, 0 ).rgb();
							result.m_direction() = normalize( texelFetch( c3d_sLights, offset++, 0 ).rgb() );
							result.m_exponentCutOff() = texelFetch( c3d_sLights, offset++, 0 );
							auto v4MtxCol1 = m_writer.declLocale( cuT( "v4MtxCol1" ), texelFetch( c3d_sLights, offset++, 0 ) );
							auto v4MtxCol2 = m_writer.declLocale( cuT( "v4MtxCol2" ), texelFetch( c3d_sLights, offset++, 0 ) );
							auto v4MtxCol3 = m_writer.declLocale( cuT( "v4MtxCol3" ), texelFetch( c3d_sLights, offset++, 0 ) );
							auto v4MtxCol4 = m_writer.declLocale( cuT( "v4MtxCol4" ), texelFetch( c3d_sLights, offset++, 0 ) );
							result.m_transform() = mat4( v4MtxCol1, v4MtxCol2, v4MtxCol3, v4MtxCol4 );
						}
					}
					else
					{
						CASTOR_EXCEPTION( "TBOS are required" );
					}

					m_writer.returnStmt( result );
				}
				, InInt{ &m_writer, cuT( "index" ) } );
		}

		Light LightingModel::getBaseLight( Type const & value )const
		{
			return writeFunctionCall< Light >( &m_writer, cuT( "getBaseLight" ), value );
		}
	}
}
