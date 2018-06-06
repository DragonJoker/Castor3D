#include "GlslLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"
#include "GlslLight.hpp"

using namespace castor;
using namespace glsl;

#define C3D_DebugLightBuffer 0

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

		String toString( FragmentInput const & value )
		{
			StringStream result{ makeStringStream() };
			result << toString( value.m_vertex ) << ", ";
			result << toString( value.m_normal );
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

		void LightingModel::declareDirectionalModel( ShadowType shadows
			, bool volumetric
			, uint32_t & index )
		{
			if ( shadows != ShadowType::eNone )
			{
				m_shadowModel->declareDirectional( shadows, index );
			}

			doDeclareLight();
			doDeclareDirectionalLight();
			doDeclareDirectionalLightUbo();
			doDeclareModel();
			doDeclareComputeOneDirectionalLight( shadows, volumetric );
		}

		void LightingModel::declarePointModel( ShadowType shadows
			, bool volumetric
			, uint32_t & index )
		{
			if ( shadows != ShadowType::eNone )
			{
				m_shadowModel->declarePoint( shadows, index );
			}

			doDeclareLight();
			doDeclarePointLight();
			doDeclarePointLightUbo();
			doDeclareModel();
			doDeclareComputeOnePointLight( shadows, volumetric );
		}

		void LightingModel::declareSpotModel( ShadowType shadows
			, bool volumetric
			, uint32_t & index )
		{
			if ( shadows != ShadowType::eNone )
			{
				m_shadowModel->declareSpot( shadows, index );
			}

			doDeclareLight();
			doDeclareSpotLight();
			doDeclareSpotLightUbo();
			doDeclareModel();
			doDeclareComputeOneSpotLight( shadows, volumetric );
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
			lightDecl.declMember< Vec4 >( cuT( "m_volumetric" ) );
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
#if C3D_DebugLightBuffer
				result.m_colourIndex() = vec4( 1.0_f, 1.0, 1.0, -1.0 );
				result.m_intensityFarPlane() = vec4( 0.8_f, 1.0, 1.0, 0.0 );
#else
				if ( m_writer.hasTexelFetch() )
				{
					if ( m_writer.hasTextureBuffers() )
					{
						auto c3d_sLights = m_writer.getBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
						auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) );
						result.m_colourIndex() = texelFetch( c3d_sLights, offset++ );
						result.m_intensityFarPlane() = texelFetch( c3d_sLights, offset++ );
						result.m_volumetric() = texelFetch( c3d_sLights, offset++ );
					}
					else
					{
						auto c3d_sLights = m_writer.getBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
						auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) );
						result.m_colourIndex() = texelFetch( c3d_sLights, offset++, 0 );
						result.m_intensityFarPlane() = texelFetch( c3d_sLights, offset++, 0 );
						result.m_volumetric() = texelFetch( c3d_sLights, offset++, 0 );
					}
				}
				else
				{
					CASTOR_EXCEPTION( "TBOS are required" );
				}
#endif
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

#if C3D_DebugLightBuffer
					result.m_direction() = vec3( 0.0_f, -0.7071068287, 0.7071067691 );
					result.m_transform() = mat4( vec4( 1.0_f, 0.0, 0.0, 0.0 )
						, vec4( 0.0_f, 1.0, 0.0, 0.0 )
						, vec4( 0.0_f, 0.0, 1.0, 0.0 )
						, vec4( 0.0_f, 0.0, 0.0, 1.0 ) );
#else
					if ( m_writer.hasTexelFetch() )
					{
						if ( m_writer.hasTextureBuffers() )
						{
							auto c3d_sLights = m_writer.getBuiltin< SamplerBuffer >( cuT( "c3d_sLights" ) );
							auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
							result.m_direction() = normalize( texelFetch( c3d_sLights, offset++ ).rgb() );
							result.m_transform() = mat4( texelFetch( c3d_sLights, offset + 0_i )
								, texelFetch( c3d_sLights, offset + 1_i )
								, texelFetch( c3d_sLights, offset + 2_i )
								, texelFetch( c3d_sLights, offset + 3_i ) );
							offset += 4_i;
						}
						else
						{
							auto c3d_sLights = m_writer.getBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
							auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
							result.m_direction() = normalize( texelFetch( c3d_sLights, offset++, 0 ).rgb() );
							result.m_transform() = mat4( texelFetch( c3d_sLights, offset + 0_i, 0 )
								, texelFetch( c3d_sLights, offset + 1_i, 0 )
								, texelFetch( c3d_sLights, offset + 2_i, 0 )
								, texelFetch( c3d_sLights, offset + 3_i, 0 ) );
							offset += 4_i;
						}
					}
					else
					{
						CASTOR_EXCEPTION( "TBOS are required" );
					}
#endif
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
							result.m_position() = texelFetch( c3d_sLights, offset++ ).rgb();
							result.m_attenuation() = texelFetch( c3d_sLights, offset++ ).rgb();
							result.m_direction() = normalize( texelFetch( c3d_sLights, offset++ ).rgb() );
							result.m_exponentCutOff() = texelFetch( c3d_sLights, offset++ );
							result.m_transform() = mat4( texelFetch( c3d_sLights, offset + 0_i )
								, texelFetch( c3d_sLights, offset + 1_i )
								, texelFetch( c3d_sLights, offset + 2_i )
								, texelFetch( c3d_sLights, offset + 3_i ) );
						}
						else
						{
							auto c3d_sLights = m_writer.getBuiltin< Sampler1D >( cuT( "c3d_sLights" ) );
							auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxLightComponentsCount ) + Int( BaseLightComponentsCount ) );
							result.m_position() = texelFetch( c3d_sLights, offset++, 0 ).rgb();
							result.m_attenuation() = texelFetch( c3d_sLights, offset++, 0 ).rgb();
							result.m_direction() = normalize( texelFetch( c3d_sLights, offset++, 0 ).rgb() );
							result.m_exponentCutOff() = texelFetch( c3d_sLights, offset++, 0 );
							result.m_transform() = mat4( texelFetch( c3d_sLights, offset + 0_i, 0 )
								, texelFetch( c3d_sLights, offset + 1_i, 0 )
								, texelFetch( c3d_sLights, offset + 2_i, 0 )
								, texelFetch( c3d_sLights, offset + 3_i, 0 ) );
							offset += 4_i;
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
